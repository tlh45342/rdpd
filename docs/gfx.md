# GFX surface layer (sprint 1)

`include/gfx.h` exposes an RGB565 pixel surface with caller-owned storage, width, height and stride (in pixels). `src/gfx.c` implements clipped rectangles, clear and the existing 5x7 doubled-size uppercase bitmap font. No RDP protocol dependency exists in GFX.

`desktop.c` now renders exclusively through `gfx_*` functions against a surface supplied by `rdpd_desktop_set_surface()`. `framebuffer.c` owns the backing pixels and serializes them into the existing RDP bitmap update format. The current desktop and input/redraw behavior are unchanged.

This is an in-process boundary, **not** yet the independent desktop process or WM. Next: explicit per-session surface lifecycle, WM hit-testing and event dispatch, then a separate desktop process with IPC.
