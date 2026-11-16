# Build 0029 — Session diagnostics
RDPD writes UTC timestamped event records to `/var/log/rdpd.log` on Linux and `rdpd.log` in the working directory on Windows. Override with `RDPD_LOG_FILE=/path/to/file`. Log open failures fall back to stderr without preventing startup. The file is opened in append mode, flushed after every event, and closed on orderly server shutdown. Configure logrotate separately for production deployments.

Each event has `level`, `session`, `event`, and optional structured key-value fields. Session IDs are monotonically assigned within a server process (not globally persistent). Events include startup, connection address, X.224 negotiation, security rejection, activation, idle timeout, protocol error, receive-end reason, session summary, and shutdown. Raw packet contents, passwords, and individual mouse movements are not logged.

Socket receive status now distinguishes EOF (-3), idle timeout (-2), generic I/O error (-1), and truncated packet (-4). Once active, a timeout while waiting for the next TPKT header is an idle event and the connection stays open. A timeout halfway through a TPKT is treated as a protocol framing failure and terminates the connection. Initial handshake still has a 3-second timeout. There is no synthetic heartbeat; a dead peer may not be detected until the next actual I/O failure.

The existing console diagnostics remain for now. Future work: per-session duration and byte counters, rate-limited idle events, robust log rotation/reopening, structured socket error codes, and a long-running idle regression test.
