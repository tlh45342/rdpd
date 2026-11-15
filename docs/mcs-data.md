# MCS SendDataRequest diagnostic — build 0007

The server decodes T.125 SendDataRequest (0x64) after AttachUser and
ChannelJoin. It validates TPKT/X.224 framing, initiator (1002), supported
channel (1002 or 1003), PER one- or two-byte payload length, and total
length. It reports priority, segmentation, payload length, candidate RDP
security flags, and a bounded 64-byte hex preview for non-sensitive packets.
Client Info and encrypted payloads are redacted.

A security-header flag of 0x0040 suggests SEC_INFO_PKT, the Client Info
PDU. This is a diagnostic inference, not a complete security or client-info
parser. No credentials are decoded, stored, or logged. The server sends no
response to SendDataRequest yet; Windows may disconnect or time out.

The synthetic 335-byte regression fixture exercises the two-byte PER length
(320 payload bytes). Live Windows interoperability still requires testing.
The service does not implement TLS/NLA, authentication, licensing, or RDP
activation. Use only on a trusted test network.
