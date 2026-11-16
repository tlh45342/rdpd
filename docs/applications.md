# Build 0024: Application registry and calculator

The desktop icon at (850,95) selects on the first mouse-down and activates on a second mouse-down while selected. This is a two-click activation gesture, not yet a timed OS-style double-click. Clicking elsewhere clears selection.

The RDP-independent app registry maps identifiers to launch callbacks. The calculator implements integer arithmetic, a 4x4 button grid, clear and division-by-zero indication, with no RDP protocol references. Its buttons use the existing widget painter.

Launching creates a calculator window. Activating the icon again raises or restores an existing window. Closing a calculator window leaves its WM slot allocated; repeated close/relaunch is currently bounded by WM_MAX_WINDOWS. Slot reuse and timed double-click recognition are follow-ups.

Run make test on Windows and Linux. Connect using mstsc at 1024x768 and activate the CALCULATOR icon at the upper right.
