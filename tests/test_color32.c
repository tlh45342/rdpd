#include "gfx.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
int main(void)
{
 uint32_t pixels[16]={0};
 gfx_surface s={pixels,4,4,4};
 gfx_rect_rgb(&s,0,0,4,4,0x123456);
 assert(pixels[0]==0x123456 && pixels[15]==0x123456);
 gfx_rect(&s,1,1,1,1,0xf800);
 assert(pixels[5]==0xff0000);
 assert(gfx_from_rgb565(0x07e0)==0x00ff00);
 assert(gfx_from_rgb565(0x001f)==0x0000ff);
 assert(gfx_blend_rgba(0x102030,0xff000000)==0x102030);
 assert(gfx_blend_rgba(0x102030,0xff0000ff)==0xff0000);
 assert(gfx_blend_rgba(0x000000,0xffffff80)==0x808080);
 puts("PASS: 32-bit RGB surfaces, RGB565 conversion, alpha blend");
 return 0;
}
