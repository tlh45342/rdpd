# rdpd

Experimental C RDP daemon. Version 0.0.1 build 0001.

Sprint 001 implements only a TCP connection listener; it is **not yet an RDP server**.
For safety it listens on loopback (127.0.0.1), TCP port 3389 by default.

Build: `make` (MinGW GCC on Windows, or GCC/Clang on POSIX).

Run: `./rdpd` or `rdpd.exe`; optional port argument: `rdpd.exe 3390`.

Test: `make test` (requires Python 3).

Next: X.224/TPKT RDP negotiation.
