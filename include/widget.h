#ifndef RDPD_WIDGET_H
#define RDPD_WIDGET_H
#include "gfx.h"
typedef struct widget {
 int x,y,width,height;
 const char *label,*active_label;
 uint16_t normal_color,active_color;
} widget;
widget widget_button(int x,int y,int width,int height,const char *label,const char *active_label,uint16_t normal,uint16_t active);
int widget_hit(const widget *w,int x,int y);
void widget_draw(const widget *w,gfx_surface *surface,int active);
void widget_draw_mac(const widget *w,gfx_surface *surface,int active);
#endif
