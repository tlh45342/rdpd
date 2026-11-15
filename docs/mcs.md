# MCS response prototype (build 0004)

This build sends a BER-encoded T.125 Connect-Response after receiving a
framed MCS Connect-Initial, then waits for one additional TPKT packet.
It is an **experimental diagnostic response**, not a complete RDP server.

The GCC ConferenceCreateResponse userData is currently empty. A compliant
RDP response needs GCC server core, security, and network data, channel IDs,
and further MCS attach/join handling. Windows Remote Desktop is expected to
reject this incomplete response. Do not interpret the server's 'sending'
log as a completed session or as evidence of client acceptance.

The current MCS initial parser validates BER framing, but does not yet
validate the full DomainParameters or GCC contents. The regression fixture
uses synthetic BER content, not a full client MCS message.

Next: implement GCC ConferenceCreateResponse, parse client GCC data,
and add interoperability tests against an actual Windows RDP client.
