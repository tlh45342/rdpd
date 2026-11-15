# Build 0011: first Demand Active / Confirm Active milestone

After the valid-client licensing status, RDPD sends a Server Demand Active
Share Control PDU on MCS global channel 1003. The synthetic desktop is
advertised as 800x600 at 16 bits per pixel, with General and Bitmap
capability sets. No OS desktop or login session is created.

The server then recognizes the Client Confirm Active Share Control PDU
and logs its share ID. This is only the first activation exchange; RDPD
does not yet implement Synchronize, Control, Font Map, bitmap updates,
or persistent session operation. The connection may still fail in mstsc.

The protocol test reads complete TPKT frames, verifies Demand Active
framing and lengths, and sends a synthetic Confirm Active fixture.

Real Windows client test: pull, build, start in foreground, connect via
mstsc, and capture logs from licensing onward. Specifically look for
`Server Demand Active sent` and `Client Confirm Active`.
