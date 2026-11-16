# Build 0033 — Outline window dragging

The window manager now supports an outline-only drag preview. On pointer-down in the title bar it captures the window position; subsequent pointer moves update only a rectangular overlay and do not move the window or recompose its client contents. Pointer-up commits the final position once. This avoids repainting a moving window's content on every mouse event.

Set `RDPD_DRAG_MODE=outline` (default) or `RDPD_DRAG_MODE=live` to choose behavior. Resize remains live. The outline is drawn in white on the existing 32-bit surface, with clipping handled by `gfx_rect_rgb`. The desktop redraws on pointer-up so the committed window position is visible. Existing window control and click handling is preserved.

Regression: `tests/test_outline.c` checks that preview motion does not modify window coordinates, that the outline is visible, that release commits coordinates, and that live mode remains functional.

This is a UI rendering optimization, **not yet a network bandwidth optimization**: the RDP transport currently sends all framebuffer tiles whenever a redraw is requested. A future sprint should add dirty rectangles / selective bitmap tile transmission, and a shared framebuffer bridge to the external session arbitrator. The arbitrator already owns a private surface, but RDPD does not yet consume it.
