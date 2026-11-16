#include "desktop.h"
#include "gfx.h"

/* The desktop uses GFX surfaces, not RDP transport primitives. */
static int clicked;
static gfx_surface *canvas;
void rdpd_desktop_set_surface(gfx_surface *surface) { canvas=surface; }
static void text(int x,int y,const char *str,unsigned short color)
{
    gfx_text(canvas,x,y,str,color);
}
void rdpd_desktop_draw(void)
{
    if (!canvas) return;
    gfx_clear(canvas,0x2190); /* blue */
    gfx_rect(canvas,0,560,800,40,0x1082);
    gfx_rect(canvas,12,566,95,27,0x4a69);
    text(23,572,"START",0xffff);
    gfx_rect(canvas,135,85,530,395,0x0000);
    gfx_rect(canvas,139,89,522,387,0xce79);
    gfx_rect(canvas,139,89,522,38,0x19a7);
    text(158,100,"RDPD DESKTOP",0xffff);
    gfx_rect(canvas,175,170,450,260,0xffff);
    text(197,195,"WELCOME TO RDPD",0x0000);
    text(197,230,"SYNTHETIC REMOTE DESKTOP",0x0000);
    gfx_rect(canvas,270,310,260,54,clicked?0x07e0:0x3a8e);
    text(322,330,clicked?"CLICKED":"CLICK ME",0xffff);
    text(150,570,"RDPD 0.0.1",0xffff);
}
int rdpd_desktop_click(unsigned x,unsigned y)
{
    if (x>=270 && x<530 && y>=310 && y<364) {
        clicked=!clicked;
        rdpd_desktop_draw();
        return 1;
    }
    return 0;
}
