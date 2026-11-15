# rdpd

Experimental C RDP daemon. Version 0.0.1 build 0001.

Sprint 001 implements only a TCP connection listener; it is **not yet an RDP server**.
It listens on all IPv4 interfaces (0.0.0.0), TCP port 3389 by default. This prototype has no authentication; use only on a trusted network and do not expose it to the public internet.

Build: `make` (MinGW GCC on Windows, or GCC/Clang on POSIX).

Run: `./rdpd` or `rdpd.exe`; optional port argument: `rdpd.exe 3390`.

Test: `make test` (requires Python 3).

Next: X.224/TPKT RDP negotiation.


https://github.com/tlh45342/rdpd/
