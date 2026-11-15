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
        payload = bytes.fromhex("40 00 00 00") + bytes(316)
        send_data = (bytes.fromhex("03 00 01 4f 02 f0 80 64 00 01 03 eb 70 81 40") + payload)
        assert len(send_data) == 335
        s.sendall(send_data)
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
    print(out)
    if err:
        print(err)
