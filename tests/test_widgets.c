#include "widget.h"
#include "icon.h"
#include <assert.h>
#include <stdio.h>
int main(void)
{
 uint32_t pixels[120*120];
 gfx_surface s={pixels,120,120,120};
 widget b=widget_button(10,10,90,40,"PRESS","ACTIVE",0x1234,0x5678);
 gfx_clear(&s,0);
 assert(widget_hit(&b,10,10));
 assert(widget_hit(&b,99,49));
 assert(!widget_hit(&b,100,49));
 assert(!widget_hit(&b,9,10));
 widget_draw(&b,&s,0);
 assert(pixels[11*120+11]==gfx_from_rgb565(0x1234));
 widget_draw(&b,&s,1);
 assert(pixels[11*120+11]==gfx_from_rgb565(0x5678));
 icon_draw_desktop(&s,60,60);
 assert(pixels[60*120+60]==gfx_from_rgb565(0x39e7));
 assert(pixels[66*120+66]==gfx_from_rgb565(0x041f));
 assert(pixels[103*120+74]==gfx_from_rgb565(0xc618));
 puts("PASS: reusable button geometry, state painting, and 48x48 desktop icon");
 return 0;
}
