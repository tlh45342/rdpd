# MCS domain establishment — build 0006

After GCC Connect-Response, the server now accepts an MCS ErectDomainRequest
(opcode 0x04), an AttachUserRequest (0x28), and ChannelJoinRequests (0x38).
It sends AttachUserConfirm assigning user ID 1002 and ChannelJoinConfirm
for the user channel 1002 and global channel 1003. The PER wire encoding
subtracts the T.125 user ID lower bound (1001).

The handler validates TPKT and X.224 framing, message lengths, the
expected sequence, and channel/user IDs. It caps diagnostic processing at
12 messages per connection. It currently rejects additional static virtual
channels rather than inventing channel assignments.

The client may next send an MCS SendDataRequest (0x64), which is not yet
handled. Legacy RDP security, client-info processing, licensing, and
activation remain outstanding. Tests use synthetic protocol packets and
do not establish Windows interoperability. Test only on a trusted LAN.
