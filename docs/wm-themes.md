# Window manager and themes â€” build 0018

WM (`include/wm.h`, `src/wm.c`) is independent of the RDP transport. It owns a bounded window registry, geometry, visibility, topmost-first hit-testing, focus index, window-local mouse callbacks and theme-driven chrome painting using GFX.

## Theme selection

Set `RDPD_THEME` **before launching the server** to one of: `classic` (default), `x11`, `win95`, `win10`, `win11`, `mac`.

Linux example: `RDPD_THEME=mac ./rdpd`

Windows PowerShell example: `$env:RDPD_THEME='win95'; .\rdpd.exe`

These are first-pass visual interpretations, not exact OS replicas. Mac theme places three colored controls at the left; other themes place controls on the right. The controls are currently decorative; close/minimize/maximize actions and drag/resize are not yet implemented. Theme names, palette, frame width, title height, control layout are data in `wm.c`, not application code. Changing theme currently requires restarting RDPD.

## Integration

`desktop.c` creates a WM window and receives window-local pointer events. Its button remains application-owned. The RDP protocol and bitmap encoder are unchanged. This is still an in-process WM; session isolation, composition, damage tracking, independent desktop processes and IPC are future work.

Run `make test` for the standalone WM tests and the existing RDP input/redraw regressions.

## Pointer capture (build 0019)

The WM now supports dragging a window by its title bar and resizing via its bottom-right 12×12 grip. The RDP input decoder forwards pointer down, movement and button release. Window content follows the window origin. Mouse capture is released on button-up. The demo uses a fixed-size 260×54 button and is not yet a general widget toolkit. Close/minimize/maximize controls remain decorative.
