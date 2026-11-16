"""End-to-end reusable client regression against a temporary RDPD server."""
import os
import socket
import subprocess
import sys
import time

from rdpclient import RDPClient

exe = os.path.abspath("rdpd.exe" if os.name == "nt" else "rdpd")
with socket.socket() as probe:
    probe.bind(("127.0.0.1", 0))
    port = probe.getsockname()[1]
server = subprocess.Popen([exe, str(port)], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
try:
    for _ in range(40):
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=.2):
                break
        except OSError:
            time.sleep(.1)
    else:
        raise RuntimeError("server did not start")
    with RDPClient("127.0.0.1", port) as client:
        client.activate()
        assert (client.width, client.height) == (1024,768)
        assert client.pixel(200,200) == 0xffff
        client.click(390,340)
        assert client.pixel(400,340) == 0xf800
        print("PASS: reusable RDP client activation, framebuffer and mouse redraw")
finally:
    server.terminate()
    server.communicate(timeout=6)
