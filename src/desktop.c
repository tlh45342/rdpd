#include "desktop.h"
#include "gfx.h"
#include "wm.h"
#include <stdlib.h>

/* Desktop owns application content; WM owns window geometry and hit-testing. */
static int clicked;
static gfx_surface *canvas;
static wm manager;
static wm_window *main_window;
static void button_click(wm_window *window,int x,int y,void *userdata)
{
 (void)window;(void)userdata;
 /* Button is positioned in window-local coordinates. */
 if(x>=135 && x<395 && y>=225 && y<279) clicked=!clicked;
}
void rdpd_desktop_set_surface(gfx_surface *surface)
{
 const char *requested=getenv("RDPD_THEME");
 wm_theme_id theme=WM_THEME_CLASSIC;
 if(requested) (void)wm_theme_parse(requested,&theme);
 canvas=surface;
 wm_init(&manager,surface,theme);
 main_window=wm_window_create(&manager,135,85,530,395,
                              "RDPD DESKTOP",button_click,0);
}
void rdpd_desktop_draw(void)
{
 if(!canvas || !main_window) return;
 gfx_clear(canvas,0x2190);
 gfx_rect(canvas,0,560,800,40,0x1082);
 gfx_rect(canvas,12,566,95,27,0x4a69);
 gfx_text(canvas,23,572,"START",0xffff);
 wm_draw(&manager);
 gfx_rect(canvas,175,170,450,260,0xffff);
 gfx_text(canvas,197,195,"WELCOME TO RDPD",0x0000);
 gfx_text(canvas,197,230,"SYNTHETIC REMOTE DESKTOP",0x0000);
 gfx_rect(canvas,270,310,260,54,clicked?0x07e0:0x3a8e);
 gfx_text(canvas,322,330,clicked?"CLICKED":"CLICK ME",0xffff);
 gfx_text(canvas,150,570,"RDPD 0.0.1",0xffff);
}
int rdpd_desktop_click(unsigned x,unsigned y)
{
 int before=clicked;
 (void)wm_mouse_down(&manager,(int)x,(int)y);
 if(clicked!=before) {
  rdpd_desktop_draw();
  return 1;
 }
 return 0;
}
