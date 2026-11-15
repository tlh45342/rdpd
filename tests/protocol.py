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
        print("PASS: experimental MCS Connect-Response received")
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
    print(out)
    if err:
        print(err)
