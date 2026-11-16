#include "desktop.h"
#include <assert.h>
#include <stdio.h>
static uint32_t pixels[1024*768];
int main(void)
{
 gfx_surface surface={pixels,1024,768,1024};
 rdpd_desktop_set_surface(&surface);
 rdpd_desktop_draw();
 assert(rdpd_desktop_pointer_at(855,105,1,1000));
 assert(pixels[91*1024+846]==gfx_from_rgb565(0x07ff));
 assert(rdpd_desktop_pointer_at(855,105,1,1600));
 assert(pixels[300*1024+900]==gfx_from_rgb565(0x2190));
 assert(rdpd_desktop_pointer_at(855,105,1,1700));
 assert(pixels[300*1024+900]!=gfx_from_rgb565(0x2190));
 puts("PASS: timed desktop double-click rejects slow click and launches on fast click");
 return 0;
}
