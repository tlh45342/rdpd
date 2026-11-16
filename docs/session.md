# Build 0025: session boundary and window lifecycle

`session.h` and `session.c` define an in-process session boundary. The session owns the window manager, application registry, calculator state, and application lifecycle. The desktop remains responsible for wallpaper, taskbar, icon presentation, and routing user input to the session. The RDP transport is independent of application and WM internals.

The desktop icon uses a monotonic clock for double-click recognition: two mouse-down events within 500ms and 8px activate the icon; a slow second click restarts the selection sequence. `rdpd_desktop_pointer_at` injects time for deterministic tests.

`wm_window_create` reuses closed slots, clearing stale callbacks and state, preserving the bounded window table and existing live window handles. Session tests verify 100 close/relaunch cycles without growing the window count.

This is not yet an IPC or multi-process session arbitrator. The session is linked into the RDPD process. A future sprint can extract it behind transport-neutral IPC and separate binaries.
