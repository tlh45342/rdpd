#ifndef RDPD_DESKTOP_H
#define RDPD_DESKTOP_H
#include "gfx.h"
void rdpd_desktop_set_surface(gfx_surface *surface);
void rdpd_desktop_draw(void);
int rdpd_desktop_click(unsigned x,unsigned y);
#endif
