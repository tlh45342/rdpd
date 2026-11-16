# Build 0034 — Selective RDP bitmap updates

The server now maintains a shadow copy of the last successfully transmitted 32-bit framebuffer, with a validity bit per 64x32 tile. Activation sends all 384 tiles; each later redraw checks tile pixels and sends only tiles that differ. Each tile is committed to the shadow copy only after its bitmap packet has been successfully sent. The shadow validity is reset for each new connection.

The change preserves the original RDP bitmap tile encoder and on-wire pixel format. It reduces network traffic on small redraws but still compares the entire desktop tile-by-tile; future optimizations can mark damage at drawing time instead of comparing memory. No changes to RDP negotiation, security, or desktop geometry.

The synthetic RDP client now composites sparse tile updates into its persistent framebuffer. The protocol integration test checks that a button click sends fewer than 384 tiles, and tests/test_dirty.c verifies unchanged tile suppression, boundary-crossing updates, and reset.

## Process boundary status

The external session_arbitrator from build 0032 still owns a separate, private framebuffer and accepts application/window/pointer IPC commands. RDPD currently renders its own in-process desktop. **Shared-memory framebuffer export/import and an RDP-to-arbitrator client are not implemented in this build.** These should be the next isolated sprint, with explicit ownership, frame generation, safe buffer synchronization, peer credential checks, and restart/failure behavior tested before switching the live server.
