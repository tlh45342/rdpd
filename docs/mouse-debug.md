# Build 0015: mouse hit-testing diagnostics

A real Windows mstsc session displayed the synthetic desktop but clicking
CLICK ME appeared to do nothing. The earlier log said 'Desktop mouse click
received' for any mouse-down event, including clicks outside the button.

The mouse handler now logs the actual RDP coordinates and whether the
button's hit-test accepted the click. A redraw is requested only when the
button state changes. Regression coverage verifies not only that 247 bitmap
packets were sent but that the button's RGB565 pixels changed to green.

This confirms the synthetic path, not yet real mstsc interactivity. On
Ubuntu run make clean && make && make test, then connect and click the
button. Look for 'Mouse down at (x,y): button activated' followed by
'framebuffer sent'. If coordinates differ from the visible button, the
next task is client coordinate mapping or input event interpretation.
