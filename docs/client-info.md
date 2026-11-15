# Client Info diagnostic and development authentication — build 0008

After MCS SendDataRequest with SEC_INFO_PKT, the server parses the
TS_INFO_PACKET fixed header and five length-delimited Unicode fields.
The development-only account is root / toor. Credentials are compared
in memory; they are never logged or hex-dumped. Malformed structures,
non-Unicode fields, and encrypted Client Info are rejected.

A local authentication success is NOT an RDP login confirmation. This
build does not yet send licensing, Demand Active, or desktop activation
PDUs. Windows will therefore still time out or disconnect. The client
may include extended info after the five strings; this build bounds but
does not interpret that extension.

This is deliberately insecure development authentication. Run only on
an isolated test network, never exposed to the internet.
