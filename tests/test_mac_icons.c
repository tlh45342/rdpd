#include "mac_icons.h"
#include "widget.h"
#include "wm.h"
#include <assert.h>
#include <stdio.h>
static uint32_t pixels[120*120];
int main(void)
{
 gfx_surface s={pixels,120,120,120};
 widget b=widget_button(4,4,80,30,"TEST","ACTIVE",0,0);
 wm manager;
 unsigned i;
 const mac_icon_id ids[]={MAC_ICON_DISK,MAC_ICON_TRASH,MAC_ICON_CALCULATOR};
 for(i=0;i<3;i++) {
  unsigned changed=0,x,y;
  gfx_clear(&s,0x2190);
  mac_icon_draw(&s,20,20,ids[i]);
  for(y=20;y<68;y++) for(x=20;x<68;x++)
   if(pixels[y*120+x]!=gfx_from_rgb565(0x2190))changed++;
  assert(changed>100);
  assert(pixels[0]==gfx_from_rgb565(0x2190));
 }
 widget_draw_mac(&b,&s,0);
 assert(pixels[5*120+20]!=gfx_from_rgb565(0x2190));
 wm_init(&manager,&s,WM_THEME_MAC);
 assert(wm_theme_get(manager.theme)->controls_left);
 assert(wm_window_create(&manager,5,5,100,90,"MAC",0,0));
 wm_draw(&manager);
 puts("PASS: 3 RGB565 mac icon resources, Mac widget, and Mac WM frame");
 return 0;
}
