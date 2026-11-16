#include "gfx.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
int main(void)
{
    uint16_t pixels[8*8];
    gfx_surface s={pixels,8,8,8};
    unsigned i;
    gfx_clear(&s,0x1234);
    for (i=0;i<64;i++) assert(pixels[i]==0x1234);
    gfx_rect(&s,-2,-2,4,4,0x07e0);
    assert(pixels[0]==0x07e0 && pixels[1]==0x07e0);
    assert(pixels[8]==0x07e0 && pixels[9]==0x07e0);
    assert(pixels[2]==0x1234 && pixels[16]==0x1234);
    gfx_rect(&s,7,7,99,99,0xffff);
    assert(pixels[63]==0xffff && pixels[62]==0x1234);
    gfx_rect(&s,9,9,10,10,0x0000);
    gfx_rect(&s,0,0,-1,4,0x0000);
    assert(pixels[63]==0xffff);
    gfx_text(&s,0,0,"A",0xf800);
    assert(pixels[0]==0x07e0);
    puts("PASS: GFX RGB565 surface, clipping, text and bounds");
    return 0;
}
