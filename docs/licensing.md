# Build 0010: synthetic RDP licensing milestone

For the isolated demonstration server, blank/blank Client Info credentials
map to the internal development account root. root/blank and root/toor
are also accepted. This is not a real operating-system login.

After successful Client Info validation the server sends MCS
SendDataIndication containing SEC_LICENSE_PKT and licensing
ERROR_ALERT with STATUS_VALID_CLIENT / ST_NO_TRANSITION.

The regression suite verifies the exact 34-byte framed response against
a synthetic Client Info request. This does not yet implement Demand Active,
Confirm Active, control/synchronize/font-map, or framebuffer updates.
Windows interoperability is not yet proven; the client may still disconnect.
Use only on an isolated development network.
