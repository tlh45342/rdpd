#ifndef RDPD_FRAMEBUFFER_H
#define RDPD_FRAMEBUFFER_H
#include <stddef.h>
#define RDPD_WIDTH 800
#define RDPD_HEIGHT 600
#define RDPD_TILE_WIDTH 64
#define RDPD_TILE_HEIGHT 32
#define RDPD_TILES_X 13
#define RDPD_TILES_Y 19
#define RDPD_TILE_COUNT (RDPD_TILES_X * RDPD_TILES_Y)
/* Build an uncompressed 16bpp RDP Bitmap Update for one desktop tile. */
int rdpd_build_bitmap_tile(unsigned index, unsigned char *out,
                           size_t capacity, size_t *used);
#endif
