#include "wm.h"
#include <assert.h>
#include <stdio.h>
static int calls,local_x,local_y;
static void on_click(wm_window *w,int x,int y,void *data)
{
 (void)w;(void)data;
 calls++;local_x=x;local_y=y;
}
int main(void)
{
 unsigned short pixels[200*160];
 gfx_surface surface={pixels,200,160,200};
 wm manager;
 wm_window *a,*b;
 wm_theme_id theme;
 unsigned i;
 for(i=0;i<WM_THEME_COUNT;i++) {
  const wm_theme *style=wm_theme_get((wm_theme_id)i);
  assert(style && style->name && style->title_height>0);
  assert(wm_theme_parse(style->name,&theme) && theme==(wm_theme_id)i);
  wm_init(&manager,&surface,theme);
  a=wm_window_create(&manager,10,10,100,100,"FIRST",on_click,0);
  b=wm_window_create(&manager,50,30,100,100,"SECOND",on_click,0);
  assert(a && b);
  assert(wm_hit_test(&manager,60,50)==1);
  assert(wm_hit_test(&manager,20,20)==0);
  assert(wm_hit_test(&manager,190,150)==-1);
  gfx_clear(&surface,0);
  wm_draw(&manager);
  assert(pixels[10*200+10]!=0 || style->frame==0);
  calls=0;
  assert(wm_mouse_down(&manager,60,50));
  assert(calls==1 && local_x==10 && local_y==20);
  assert(!wm_mouse_down(&manager,190,150));
  assert(calls==1);
  b->visible=0;
  assert(wm_hit_test(&manager,60,50)==0);
  assert(!wm_theme_parse("unknown",&theme));
 }
 wm_init(&manager,&surface,WM_THEME_CLASSIC);
 for(i=0;i<WM_MAX_WINDOWS;i++)
  assert(wm_window_create(&manager,0,0,10,10,"X",0,0));
 assert(!wm_window_create(&manager,0,0,10,10,"X",0,0));
 puts("PASS: WM themes, z-order hit-testing, local events and limits");
 return 0;
}
