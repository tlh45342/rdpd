# Build 0009 development authentication

Temporary isolated-network allowance: Unicode username `root` is accepted
with either the password `toor` or an empty password. Other usernames and
nonempty passwords are rejected. Log reports credential field lengths and
match status, never password contents. This is not production security.

This only changes local Client Info validation. The server does not yet
send licensing, activation, or desktop PDUs. Windows may still disconnect.
Remove the empty-password allowance before any deployment outside an
isolated test environment.
