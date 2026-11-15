# GCC negotiation prototype (build 0005)

The server now includes a GCC ConferenceCreateResponse envelope in the BER
MCS Connect-Response userData. It advertises SC_CORE (RDP version),
SC_SECURITY (encryption disabled), and SC_NET (global channel 1003, no
static virtual channels). A bounded diagnostic scan reports the client
CS_CORE desktop size if found.

This is **not** a completed RDP session: the client GCC ConferenceCreateRequest
is not fully parsed, the server security negotiation is not implemented,
and MCS ErectDomain/AttachUser/ChannelJoin handling is missing. The client
may reject the response. The current regression uses synthetic MCS payload,
so passing tests does not prove Windows interoperability.

Run `make test`, then connect from Windows 10 to the Ubuntu listener. Record
whether a `next client TPKT` line appears. Do not expose the server to an
untrusted network: TLS/NLA are not supported.
