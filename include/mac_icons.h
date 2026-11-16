#ifndef RDPD_MAC_ICONS_H
#define RDPD_MAC_ICONS_H
#include "gfx.h"
typedef enum {MAC_ICON_DISK,MAC_ICON_TRASH,MAC_ICON_CALCULATOR} mac_icon_id;
void mac_icon_draw(gfx_surface *surface,int x,int y,mac_icon_id id);
#endif
