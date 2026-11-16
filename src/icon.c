#include "icon.h"
/* 48x48 monitor/computer icon, rendered using RGB565 primitives. */
void icon_draw_desktop(gfx_surface *s,int x,int y)
{
 gfx_rect(s,x,y,48,48,0x39e7);
 gfx_rect(s,x+3,y+3,42,32,0x0000);
 gfx_rect(s,x+6,y+6,36,26,0x041f);
 gfx_rect(s,x+9,y+9,30,4,0x07ff);
 gfx_rect(s,x+9,y+17,21,3,0xffff);
 gfx_rect(s,x+9,y+23,26,3,0x07e0);
 gfx_rect(s,x+21,y+35,6,7,0xc618);
 gfx_rect(s,x+13,y+42,22,4,0xc618);
}
