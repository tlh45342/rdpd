import socket
import subprocess
import time
import os
import sys

exe = os.path.abspath("rdpd.exe" if os.name == "nt" else "rdpd")
probe = socket.socket()
probe.bind(("127.0.0.1", 0))
port = probe.getsockname()[1]
probe.close()
p = subprocess.Popen([exe, str(port)], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
try:
    for attempt in range(30):
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.2):
                break
        except OSError:
            time.sleep(0.1)
    else:
        raise RuntimeError("listener did not start")
    time.sleep(0.2)
    print("PASS: TCP connection accepted on localhost")
finally:
    p.terminate()
    out, err = p.communicate(timeout=5)
    print(out, end="")
    if err:
        print(err, file=sys.stderr)
    if p.returncode not in (0, -15, 1, 3221225786):
        print("process exit:", p.returncode)
