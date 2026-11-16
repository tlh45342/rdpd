#include "framebuffer.h"
#include <assert.h>
#include <stdio.h>
int main(void)
{
 unsigned i;
 rdpd_framebuffer_dirty_reset();
 for(i=0;i<RDPD_TILE_COUNT;i++)assert(rdpd_framebuffer_tile_dirty(i));
 for(i=0;i<RDPD_TILE_COUNT;i++)rdpd_framebuffer_tile_commit(i);
 for(i=0;i<RDPD_TILE_COUNT;i++)assert(!rdpd_framebuffer_tile_dirty(i));
 rdpd_framebuffer_rect(2,2,2,2,0xf800);
 assert(rdpd_framebuffer_tile_dirty(0));
 assert(!rdpd_framebuffer_tile_dirty(1));
 assert(!rdpd_framebuffer_tile_dirty(RDPD_TILES_X));
 rdpd_framebuffer_tile_commit(0);
 assert(!rdpd_framebuffer_tile_dirty(0));
 rdpd_framebuffer_rect(63,31,3,3,0x07e0);
 assert(rdpd_framebuffer_tile_dirty(0));
 assert(rdpd_framebuffer_tile_dirty(1));
 assert(rdpd_framebuffer_tile_dirty(RDPD_TILES_X));
 assert(rdpd_framebuffer_tile_dirty(RDPD_TILES_X+1));
 rdpd_framebuffer_dirty_reset();
 for(i=0;i<RDPD_TILE_COUNT;i++)assert(rdpd_framebuffer_tile_dirty(i));
 puts("PASS: dirty tile detection, unchanged tile suppression, boundaries and reset");
 return 0;
}
