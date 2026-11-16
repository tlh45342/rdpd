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
/* Detect dirty 64x32 tiles against last transmitted frame.
   First call marks every tile. Call commit only after successful send. */
void rdpd_framebuffer_dirty_reset(void);
int rdpd_framebuffer_tile_dirty(unsigned index);
void rdpd_framebuffer_tile_commit(unsigned index);
/* Build an uncompressed 32bpp RDP Bitmap Update for one desktop tile. */
int rdpd_build_bitmap_tile(unsigned index, unsigned char *out,
                           size_t capacity, size_t *used);
#endif
