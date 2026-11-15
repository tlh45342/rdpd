# Build 0012: connection finalization

RDPD recognizes client Synchronize, Control, and Font List Share Data PDUs
following Confirm Active. Upon Font List it sends four server Share Data
PDUs: Synchronize, Control Cooperate, Control Granted, and Font Map.
The synthetic regression test verifies each response frame and subtype.

The arbitrary 12-packet diagnostic limit has been removed. Once the server
sends finalization, the socket receive idle timeout becomes 30 seconds.
This is not a periodic keepalive, and graphics updates are not yet sent.

`RDP session ACTIVE` means the local state machine reached finalization,
not that the real Windows client has accepted the exchange or displayed
a desktop. Real mstsc interoperability must be tested separately.

Run `make test`, then start the server in the foreground and connect with
Windows Remote Desktop. Capture the log from Confirm Active onward and
report whether the client stays open, shows a blank screen, or errors.
