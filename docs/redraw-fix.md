# Build 0016: explicit redraw event propagation

Real Windows mstsc showed accurate mouse hit testing and button activation
but no second `framebuffer sent` message. The protocol state machine had
used a transient state value 8 to represent a pending redraw.

The MCS domain decoder now propagates a positive event result (2) for a
changed button while leaving the active session in state 7. The connection
handler explicitly sends bitmap updates when `state == 7 && status == 2`.
This separates session lifecycle state from application redraw events.

`make test` checks a mouse click, the returned 247 update packets and
changed RGB565 button pixels. Windows mstsc still needs to verify that
clicking CLICK ME changes it to CLICKED.
