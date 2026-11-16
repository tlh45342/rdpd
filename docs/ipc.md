# Build 0030 — Session Arbitrator IPC v1

This sprint establishes a transport-neutral, versioned **binary IPC contract**, not yet a separate arbitrator process. `include/ipc.h` and `src/ipc.c` implement encoding, validation, framing, and decoding without depending on RDP, sockets, WM, or OS-specific headers.

## Wire format
All multibyte integers are **big-endian** (network byte order). Every frame begins with a 24-byte header:

| Offset | Size | Field |
|---|---:|---|
| 0 | 4 | ASCII `RDIP` |
| 4 | 2 | Protocol version = 1 |
| 6 | 2 | Message type |
| 8 | 4 | Session ID |
| 12 | 4 | Request/correlation ID |
| 16 | 4 | Payload length |
| 20 | 4 | Reserved = 0 |

Maximum payload is 65,536 bytes. Frame length is 24 + payload length. `ipc_frame_length` reports incomplete headers/frames, invalid magic/reserved fields, unsupported versions/types, and oversized frames. `ipc_decode` borrows the input payload (no heap allocation); callers must retain the input buffer while accessing it. No C pointers or compiler-dependent structures are sent over the wire.

## Messages and payloads
Every numeric payload field is a 32-bit unsigned big-endian integer, except the app identifier which is raw ASCII.

| Code | Name | Payload |
|---|---|---|
| 1 | HELLO | minimum version, maximum version (8 bytes) |
| 2 | HELLO_ACK | selected version, capabilities (8 bytes) |
| 3 | SESSION_OPEN | width, height (8 bytes) |
| 4 | SESSION_CLOSE | empty |
| 5 | POINTER | x, y, kind (12 bytes; kind 0=move, 1=down, 2=up) |
| 6 | KEY | scancode, flags (8 bytes) |
| 7 | APP_LAUNCH | 1–128 ASCII bytes: lowercase letters, digits, dash, underscore |
| 8 | WINDOW_ACTION | window handle, action (8 bytes; 1=focus, 2=minimize, 3=restore, 4=maximize, 5=close) |
| 9 | FRAME_DAMAGE | x, y, width, height (16 bytes) |
| 10 | STATUS | status, detail (8 bytes) |
| 11 | HEARTBEAT | empty |

A framebuffer damage event describes an updated rectangle; it does **not** transport pixels. The future arbitrator/desktop transport must define framebuffer ownership (e.g. shared memory plus generation counters) before the process split. Application launch identifiers are deliberately restricted to avoid interpreting arbitrary paths as executable commands.

The receiver must reject unknown types, unsupported versions, nonzero reserved fields, invalid payload sizes/values, and incomplete frames. The codec does not yet implement authentication, authorization, IPC transport, request timeouts, or session ownership. A trusted local transport and peer identity verification will be required before enabling separate processes. Message type support is fixed for v1; future additions require negotiation/versioning.

## Next steps
Build 0031: choose a concrete local transport (Unix-domain sockets on Linux and a corresponding Windows development transport), implement handshake and request correlation, then connect the session module through a dispatch adapter. Preserve the existing in-process server as a fallback until end-to-end multi-process regression tests pass.

Run `make test` to execute `tests/test_ipc.c`, covering all eleven message types, network byte order, framing, round trips, and malformed input.
