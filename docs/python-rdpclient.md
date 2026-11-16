# Python RDPD test client (Phase 1)

The reusable client lives in `tests/rdpclient/`. It implements the existing
RDPD **legacy-security synthetic fixture**, not general Windows RDP.

## Regression

Run `make test` or `python tests/test_rdpclient.py`. The integration test
starts a disposable local RDPD server, completes session activation,
reconstructs all 247 bitmap tiles, clicks the button, and verifies its
new RGB565 color (`0x07e0`).

## Screenshot CLI

Start RDPD on port 3389 in one terminal, then run:

```sh
python -m tests.rdpclient --host 127.0.0.1 --port 3389 --output desktop.png
python -m tests.rdpclient --host 127.0.0.1 --click 390 340 --output clicked.png
```

The screenshot writer uses only Python's standard library (PNG RGB conversion).
The client currently assumes the fixed 800x600 RGB565 RDPD framebuffer and
247 uncompressed bitmap rectangles per update.

## Windows Server 2019 reference

TESTSRV at 192.168.150.112 negotiates HYBRID_EX (TLS/CredSSP).
**This client cannot yet connect to TESTSRV**. Do not interpret a failed
negotiation as a server fault. TLS, CredSSP, and general RDP capability
negotiation are separate future work. No reference-server credentials are
stored in the repository.
