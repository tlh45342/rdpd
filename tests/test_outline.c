#include "wm.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void)
{
 static uint32_t pixels[640*480];
 gfx_surface surface={pixels,640,480,640};
 wm manager;
 wm_window *window;
 wm_init(&manager,&surface,WM_THEME_MAC);
 window=wm_window_create(&manager,50,50,200,150,"TEST",0,0);
 assert(window);
 wm_set_outline_drag(&manager,1);
 assert(wm_pointer_down(&manager,100,60));
 assert(wm_pointer_move(&manager,140,80));
 assert(window->x==50 && window->y==50);
 assert(manager.outline_x==90 && manager.outline_y==70);
 wm_draw(&manager);
 assert(pixels[70*640+90]==0x00ffffff);
 wm_pointer_up(&manager);
 assert(window->x==90 && window->y==70);
 assert(manager.pointer_mode==0);
 wm_set_outline_drag(&manager,0);
 assert(wm_pointer_down(&manager,180,80));
 assert(wm_pointer_move(&manager,190,100));
 assert(window->x==100 && window->y==90);
 wm_pointer_up(&manager);
 puts("PASS: outline drag previews frame only, commits on release; live mode remains available");
 return 0;
}
