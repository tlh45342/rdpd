# Mac-style desktop theme and icon resources (build 0026)

Set `RDPD_THEME=mac` to enable a light gray Mac-style desktop, Aqua-inspired rounded beveled buttons, gradient window title bars, and red/yellow/green window controls on the left. The existing classic, x11, and Windows themes remain selectable.

The desktop displays three 48x48 RGB565 icon resources: Calculator at (850,95), Disk at (850,192), and Trash at (850,289). Calculator is interactive (timed double-click launches/raises its window). Disk and Trash are decorative placeholders; they do not yet perform filesystem operations.

`resources/icons/mac_icon_assets.inc` stores three indexed-color 48x48 resources with transparent pixels. `src/mac_icons.c` composites them directly into GFX RGB565 surfaces, independent of RDP. The accompanying PNG and multi-resolution ICO assets are exported using `python tools/generate_mac_icons.py` (Pillow is needed only for asset generation, not for building or running the server). ICO entries are 16, 24, 32, and 48 pixels. This is an embedded raster resource path; runtime ICO decoding is future work.

`make test` covers existing protocols, WM, calculator, session, icon rasterization, and Mac widget/frame rendering. Use `RDPD_THEME=mac` on the server to exercise the Mac desktop.
