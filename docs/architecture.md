# Architecture

Sprint 001: A single-threaded TCP listener. No RDP handshake is implemented.

- main.c: CLI, version, signal handlers
- server.c: socket setup, accept loop, connection logging
- rdpd.h: public entry point and default port

The listener binds to 127.0.0.1 only by default to avoid exposing an unauthenticated service.
The next milestone is TPKT/X.224 connection request parsing and a standards-compliant response.
