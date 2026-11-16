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
