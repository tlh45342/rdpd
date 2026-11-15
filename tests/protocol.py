import os
import socket
import subprocess
import time

exe = os.path.abspath("rdpd.exe" if os.name == "nt" else "rdpd")
probe = socket.socket()
probe.bind(("127.0.0.1", 0))
port = probe.getsockname()[1]
probe.close()
p = subprocess.Popen([exe, str(port)], stdout=subprocess.PIPE,
                     stderr=subprocess.PIPE, text=True)

def request(data):
    with socket.create_connection(("127.0.0.1", port), timeout=3) as s:
        s.settimeout(4)
        s.sendall(data)
        return s.recv(128)

try:
    for _ in range(40):
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=.2):
                break
        except OSError:
            time.sleep(.1)
    else:
        raise RuntimeError("listener not ready")

    legacy = bytes.fromhex("03 00 00 0b 06 e0 00 00 00 00 00")
    confirm = bytes.fromhex("03 00 00 0b 06 d0 00 00 00 00 00")
    assert request(legacy) == confirm, "legacy X.224 confirmation mismatch"
    print("PASS: legacy X.224 confirmation")

    # Two-byte BER length (128 payload bytes) exercises TPKT and MCS framing.
    mcs = bytes.fromhex("03 00 00 8b 02 f0 80 7f 65 81 80") + bytes(128)
    with socket.create_connection(("127.0.0.1", port), timeout=3) as s:
        s.settimeout(4)
        s.sendall(legacy)
        assert s.recv(128) == confirm
        s.sendall(mcs)
        response = s.recv(256)
        assert response[:2] == bytes.fromhex("03 00") and response[7:9] == bytes.fromhex("7f 66"), "MCS Connect-Response missing"
        assert bytes.fromhex("4d 63 44 6e") in response, "GCC conference identifier missing"
        assert bytes.fromhex("01 0c 0c 00") in response, "SC_CORE missing"
        assert bytes.fromhex("02 0c 0c 00") in response, "SC_SECURITY missing"
        assert bytes.fromhex("03 0c 08 00 eb 03") in response, "SC_NET missing"
        assert len(response) == int.from_bytes(response[2:4], "big"), "TPKT length incorrect"
        print("PASS: MCS Connect-Response GCC server blocks and TPKT length")
        s.sendall(bytes.fromhex("03 00 00 0c 02 f0 80 04 01 00 01 00"))
        s.sendall(bytes.fromhex("03 00 00 08 02 f0 80 28"))
        assert s.recv(11) == bytes.fromhex("03 00 00 0b 02 f0 80 2e 00 00 01"), "AttachUserConfirm incorrect"
        s.sendall(bytes.fromhex("03 00 00 0c 02 f0 80 38 00 01 03 ea"))
        assert s.recv(15) == bytes.fromhex("03 00 00 0f 02 f0 80 3e 00 00 01 03 ea 03 ea"), "User channel confirm incorrect"
        s.sendall(bytes.fromhex("03 00 00 0c 02 f0 80 38 00 01 03 eb"))
        assert s.recv(15) == bytes.fromhex("03 00 00 0f 02 f0 80 3e 00 00 01 03 eb 03 eb"), "Global channel confirm incorrect"
        print("PASS: MCS erect, attach user, and channel joins")
        def client_info(username, password):
            fields = ["", username, password, "", ""]
            strings = [x.encode("utf-16le") for x in fields]
            info = ((0).to_bytes(4, "little") + (0x10).to_bytes(4, "little") +
                    b"".join(len(x).to_bytes(2, "little") for x in strings) +
                    b"".join(x + bytes(2) for x in strings))
            return bytes.fromhex("40 00 ff 30") + info
        info = client_info("root", "toor")
        payload = info + bytes(320 - len(info))
        send_data = (bytes.fromhex("03 00 01 4f 02 f0 80 64 00 01 03 eb 70 81 40") + payload)
        assert len(send_data) == 335
        s.sendall(send_data)
        def receive_tpkt(sock):
            def exact(count):
                data = bytearray()
                while len(data) < count:
                    chunk = sock.recv(count - len(data))
                    if not chunk:
                        raise EOFError("TPKT truncated")
                    data.extend(chunk)
                return bytes(data)
            header = exact(4)
            size = int.from_bytes(header[2:4], "big")
            assert header[:2] == bytes.fromhex("03 00") and size >= 7
            return header + exact(size - 4)
        license_reply = receive_tpkt(s)
        assert license_reply == bytes.fromhex("03 00 00 22 02 f0 80 68 00 01 03 eb 70 14 80 00 00 00 ff 03 10 00 07 00 00 00 02 00 00 00 00 00 00 00"), "licensing status response mismatch"
        print("PASS: RDP licensing valid-client status response")
        demand = receive_tpkt(s)
        assert demand[:2] == bytes.fromhex("03 00"), "Demand Active TPKT signature mismatch"
        assert demand[7] == 0x68 and demand[14+2:14+4] == bytes.fromhex("11 00"), "Server Demand Active missing"
        assert int.from_bytes(demand[2:4], "big") == len(demand), "Demand Active TPKT length mismatch"
        assert int.from_bytes(demand[14:16], "little") == len(demand)-14, "Demand Active share length mismatch"
        print("PASS: Server Demand Active framed with capabilities")
        confirm_share = (bytes.fromhex("14 00 13 00 ea 03") + bytes.fromhex("ea 03 01 00") + bytes(4) + bytes(6))
        confirm_mcs = bytes.fromhex("64 00 01 03 eb 70") + bytes([len(confirm_share)]) + confirm_share
        s.sendall(bytes.fromhex("03 00") + (len(confirm_mcs)+7).to_bytes(2,"big") + bytes.fromhex("02 f0 80") + confirm_mcs)
        print("PASS: sent synthetic Client Confirm Active")
        def share_data(subtype, body):
            length = 18 + len(body)
            share = (length.to_bytes(2,"little") + bytes.fromhex("17 00 ea 03 ea 03 01 00") +
                     bytes.fromhex("00 01") + length.to_bytes(2,"little") +
                     bytes([subtype,0,0,0]) + body)
            mcs = bytes.fromhex("64 00 01 03 eb 70") + bytes([len(share)]) + share
            return bytes.fromhex("03 00") + (len(mcs)+7).to_bytes(2,"big") + bytes.fromhex("02 f0 80") + mcs
        s.sendall(share_data(31,bytes.fromhex("01 00 ea 03")))
        s.sendall(share_data(20,bytes.fromhex("04 00 00 00 00 00 00 00")))
        s.sendall(share_data(20,bytes.fromhex("01 00 00 00 00 00 00 00")))
        s.sendall(share_data(39,bytes(8)))
        for subtype in (31,20,20,40):
            response = receive_tpkt(s)
            assert response[7] == 0x68 and response[14+14] == subtype, "finalization PDU mismatch"
        print("PASS: four server connection-finalization PDUs")
        # Diagnostic only: server should accept a well-framed 335-byte SendDataRequest.
        print("PASS: sent 335-byte MCS SendDataRequest diagnostic fixture")
    print("PASS: MCS Connect-Initial framed packet received")

    malformed_mcs = bytes.fromhex("03 00 00 0b 02 f0 80 7f 65 02 00")
    with socket.create_connection(("127.0.0.1", port), timeout=3) as s:
        s.settimeout(4)
        s.sendall(legacy)
        assert s.recv(128) == confirm
        s.sendall(malformed_mcs)
        assert s.recv(1) == b"", "malformed MCS not rejected"
    print("PASS: malformed MCS BER length rejected")

    tls = bytes.fromhex("03 00 00 13 0e e0 00 00 00 00 00 01 00 08 00 03 00 00 00")
    failure = bytes.fromhex("03 00 00 13 0e d0 00 00 00 00 00 03 00 08 00 02 00 00 00")
    assert request(tls) == failure, "unsupported TLS negotiation not rejected"
    print("PASS: unsupported TLS/CredSSP rejected")

    assert request(bytes.fromhex("03 00 00 04")) == b"", "short TPKT accepted"
    print("PASS: invalid TPKT rejected")
finally:
    p.terminate()
    out, err = p.communicate(timeout=6)
    assert "MCS SendDataRequest: initiator=1002 channel=1003" in out, "SendDataRequest not decoded"
    assert "SEC_INFO_PKT candidate" in out, "RDP security header not inspected"
    assert "Development authentication: ACCEPTED (root)" in out, "valid development login not accepted"
    assert "Client Confirm Active:" in out, "synthetic Confirm Active not parsed"
    assert "RDP session ACTIVE" in out, "session finalization not reached"
    print(out)
    if err:
        print(err)
