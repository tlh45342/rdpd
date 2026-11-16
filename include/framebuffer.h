#ifndef RDPD_FRAMEBUFFER_H
#define RDPD_FRAMEBUFFER_H
#include <stddef.h>
#define RDPD_WIDTH 1024
void rdpd_framebuffer_clear(unsigned short color);
void rdpd_framebuffer_rect(int x,int y,int width,int height,unsigned short color);
#define RDPD_HEIGHT 768
#define RDPD_TILE_WIDTH 64
#define RDPD_TILE_HEIGHT 32
#define RDPD_TILES_X ((RDPD_WIDTH+RDPD_TILE_WIDTH-1)/RDPD_TILE_WIDTH)
#define RDPD_TILES_Y ((RDPD_HEIGHT+RDPD_TILE_HEIGHT-1)/RDPD_TILE_HEIGHT)
#define RDPD_TILE_COUNT (RDPD_TILES_X * RDPD_TILES_Y)
/* Build an uncompressed 16bpp RDP Bitmap Update for one desktop tile. */
int rdpd_build_bitmap_tile(unsigned index, unsigned char *out,
                           size_t capacity, size_t *used);
#endif
