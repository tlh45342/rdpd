#include "widget.h"
#include <string.h>
widget widget_button(int x,int y,int width,int height,const char *label,const char *active_label,uint16_t normal,uint16_t active)
{
 widget w={x,y,width,height,label,active_label,normal,active};return w;
}
int widget_hit(const widget *w,int x,int y)
{
 return w && x>=w->x && y>=w->y && x-w->x<w->width && y-w->y<w->height;
}
void widget_draw(const widget *w,gfx_surface *s,int active)
{
 const char *label;
 int tx,ty;
 if(!w || !s) return;
 gfx_rect(s,w->x,w->y,w->width,w->height,active?w->active_color:w->normal_color);
 label=active && w->active_label?w->active_label:w->label;
 if(!label) return;
 tx=w->x+(w->width-(int)strlen(label)*12)/2;
 ty=w->y+(w->height-14)/2;
 gfx_text(s,tx,ty,label,0xffff);
}

/* Aqua-inspired beveled button. Kept separate from the neutral widget model. */
void widget_draw_mac(const widget *w,gfx_surface *s,int active)
{
 int x,y,ww,hh;
 const char *label;
 if(!w || !s || w->width<8 || w->height<8) return;
 x=w->x;y=w->y;ww=w->width;hh=w->height;
 gfx_rect(s,x+3,y,ww-6,hh,0x8410);
 gfx_rect(s,x,y+3,ww,hh-6,0x8410);
 gfx_rect(s,x+2,y+2,ww-4,hh-4,active?0x64bd:0xe71c);
 gfx_rect(s,x+4,y+3,ww-8,hh/2-3,active?0x8e3f:0xffff);
 gfx_rect(s,x+4,y+hh-5,ww-8,2,active?0x3b9a:0xb596);
 label=active && w->active_label?w->active_label:w->label;
 if(label) gfx_text(s,x+(ww-(int)strlen(label)*12)/2,
                      y+(hh-14)/2,label,0x2104);
}
