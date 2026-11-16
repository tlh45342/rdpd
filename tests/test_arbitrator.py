"""Unix socket integration test: launch arbitrator, negotiate, open, reject unhandled dispatch, close."""
import os
import socket
import struct
import subprocess
import sys
import tempfile
import time
from pathlib import Path

if os.name == "nt":
    print("SKIP: Unix socket arbitrator integration test (Windows)")
    sys.exit(0)

root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix="rdpd-ipc-") as directory:
    path = str(Path(directory) / "arbitrator.sock")
    process = subprocess.Popen([str(root / "session_arbitrator"), path],
                               stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    def send(sock, kind, sid, rid, payload=b""):
        sock.sendall(struct.pack("!4sHHIIII", b"RDIP", 1, kind, sid, rid,
                                 len(payload), 0) + payload)
    def receive(sock):
        def exact(count):
            result = b""
            while len(result) < count:
                part = sock.recv(count-len(result))
                if not part:
                    raise EOFError("arbitrator closed")
                result += part
            return result
        magic, version, kind, sid, rid, size, reserved = struct.unpack(
            "!4sHHIIII", exact(24))
        assert magic == b"RDIP" and version == 1 and reserved == 0
        return kind, sid, rid, exact(size)
    try:
        for _ in range(100):
            if os.path.exists(path):
                break
            time.sleep(0.01)
        assert os.path.exists(path), "arbitrator failed to start"
        with socket.socket(socket.AF_UNIX) as sock:
            sock.settimeout(3)
            sock.connect(path)
            send(sock, 1, 0, 1, struct.pack("!II", 1, 1))
            assert receive(sock) == (2, 0, 1, struct.pack("!II", 1, 0))
            send(sock, 3, 17, 2, struct.pack("!II", 1024, 768))
            assert receive(sock) == (10, 17, 2, b"\0"*8)
            send(sock, 7, 17, 3, b"calculator")
            assert receive(sock) == (10, 17, 3, struct.pack("!II", 0, 1))
            send(sock, 5, 17, 31, struct.pack("!III", 700, 310, 1))
            assert receive(sock)[:3] == (10, 17, 31)
            send(sock, 5, 17, 32, struct.pack("!III", 700, 310, 2))
            assert receive(sock)[:3] == (10, 17, 32)
            send(sock, 8, 17, 33, struct.pack("!II", 0, 2))
            assert receive(sock) == (10, 17, 33, struct.pack("!II", 0, 0))
            send(sock, 8, 17, 34, struct.pack("!II", 0, 3))
            assert receive(sock) == (10, 17, 34, struct.pack("!II", 0, 0))
            send(sock, 8, 17, 35, struct.pack("!II", 0, 5))
            assert receive(sock) == (10, 17, 35, struct.pack("!II", 0, 0))
            send(sock, 7, 17, 36, b"calculator")
            assert receive(sock) == (10, 17, 36, struct.pack("!II", 0, 1))
            send(sock, 7, 17, 37, b"missing")
            assert receive(sock) == (10, 17, 37, struct.pack("!II", 2, 1))
            send(sock, 11, 17, 4)
            assert receive(sock) == (10, 17, 4, b"\0"*8)
            send(sock, 4, 17, 5)
            assert receive(sock) == (10, 17, 5, b"\0"*8)
        print("PASS: arbitrator Unix socket handshake, session lifecycle, correlation, live session dispatch")
    finally:
        process.terminate()
        try:
            process.wait(timeout=3)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()
