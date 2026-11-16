# Build 0023: desktop resolution, widget foundation and icon

The RGB565 framebuffer is 1024x768. Bitmap updates use 64x32 tiles (16 columns x 24 rows, 384 total); RDP activation advertises the matching resolution. The Python development client and protocol tests use the same geometry.

`widget.h` provides a window-local button model with geometry, normal/active labels and colors, hit-testing and painting. Both demonstration windows now use this common implementation. Application state remains separate from WM. This is the first widget foundation, not yet a complete toolkit: text entry, keyboard focus, checkbox and radio widgets are future work.

`icon.h`/`icon.c` draw a 48x48 computer monitor icon on the desktop at (850,95) using only GFX RGB565 primitives. The icon is decorative for now; launching applications from icons is future work.

`make test` validates protocol geometry, RDP input and framebuffer updates, window management, widget button geometry and icon pixels. Full-frame updates and temporary window paint surfaces remain known performance limitations.
