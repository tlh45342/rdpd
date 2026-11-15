# Build 0014: synthetic desktop and first interaction

The RDP transport remains independent from the desktop renderer.
`framebuffer.c` owns an RGB565 pixel buffer and rectangle drawing;
`desktop.c` composes a taskbar, window, 5x7 glyph text and a button.
`finalization.c` decodes slow-path mouse input events, forwarding left
button presses to `rdpd_desktop_click`. Clicking the central button toggles
its label and color, and the server retransmits the framebuffer.

This is a single-process prototype. The next architectural step is an
explicit event queue and window/widget tree: RDP input decoder -> desktop
hit testing/focus -> application event handler -> dirty regions -> RDP
bitmap updates. Keep the application backend independent of RDP, so it
can later run in a separate process using Unix-domain sockets.

`make test` verifies desktop bitmap coverage and a synthetic mouse click
triggering a complete redraw. Real Windows mstsc rendering and clicking
must still be verified. Bitmap retransmission is intentionally full-screen
for now; dirty rectangle updates are future work.
