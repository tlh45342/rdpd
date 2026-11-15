# Proposed framebuffer contract (not implemented)

Start with a single logical 1024 x 768 display. Use 32-bit XRGB8888 pixels
(4 bytes per pixel, little-endian B,G,R,X in memory), top-left origin,
row-major, stride 4096 bytes, 3,145,728 bytes per frame.

Separate the producer API from the RDP transport: a faux application writes
pixels to a framebuffer, while the RDP encoder reads snapshots and sends
updates. Do not have the producer write to the network socket.

First prototype: in-process framebuffer with draw/clear/update APIs. Later:
shared-memory mapped framebuffer with a versioned header (magic, dimensions,
stride, format, generation), synchronization, and dirty rectangles. A second
TCP port is not required for pixel transport; reserve it for an optional
authenticated control API, not an unauthenticated remote drawing endpoint.

The screen cannot be displayed until later RDP capability negotiation,
licensing/security, graphics updates, and session lifecycle are implemented.
