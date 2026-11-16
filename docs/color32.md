# Build 0027: 32-bit framebuffer and RDP bitmap output

RDPD now stores each pixel in 32-bit `0x00RRGGBB` (XRGB8888) and sends 32-bit uncompressed RDP bitmap rectangles. At 1024x768 the framebuffer occupies 3,145,728 bytes, up from 1,572,864 bytes for RGB565. The 64x32 bitmap tile is 8,192 pixel bytes; the transport packet buffer is 16,384 bytes.

GFX includes `gfx_rect_rgb` for native 24-bit color in a 32-bit pixel, `gfx_from_rgb565` to preserve legacy theme constants, and `gfx_blend_rgba` for alpha compositing. Existing themes and buttons retain RGB565 color constants, converted when rendered. The Mac icon resources remain 16-entry palettes; changing the framebuffer alone does not add colors to the original artwork. PNG/ICO resource decoding and full-color image assets are future work.

The development RDP client and protocol tests now validate 32bpp bitmap headers, 4-byte bottom-up scanlines, and mouse redraw. This is uncompressed 32-bit RDP, not a compressed 32-bit graphics codec. It increases network bandwidth, and older clients that only accept 16-bit depth are not yet negotiated down.
