# Build 0013: first synthetic framebuffer

After connection finalization, RDPD generates an 800x600 16-bit RGB565
blue framebuffer in memory and transmits it as 247 uncompressed RDP
Bitmap Update PDUs on the global MCS channel. Each rectangle is at most
64x32 pixels and is encoded bottom-up with two-byte little-endian pixels.
No X11, Wayland, GPU, login session, or destination host is required.

The protocol regression test reads all 247 TPKT frames, validates the
MCS PER lengths, Share Data and bitmap headers, rectangle boundaries,
and every pixel value. It verifies that all tiles cover exactly 800x600.

This is an initial graphics implementation. Actual Windows mstsc rendering
has not been confirmed; if the client shows black, blank, or disconnects,
inspect the Windows client behavior and server logs before adding drawing.

To test on Ubuntu: git pull; make clean; make; make test; pkill -x rdpd;
./rdpd. Connect from Windows Remote Desktop using blank credentials.
Look for `framebuffer sent: 800x600 RGB565, 247 bitmap rectangles`.
