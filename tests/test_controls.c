#include "wm.h"
#include <assert.h>
#include <stdio.h>
static int allow;
static int close_request(wm_window *w,void *data)
{
 (void)w;(void)data;return allow;
}
int main(void)
{
 uint16_t pixels[800*600];
 gfx_surface s={pixels,800,600,800};
 wm m;
 wm_window *a,*b;
 wm_init(&m,&s,WM_THEME_WIN95);
 a=wm_window_create(&m,50,50,300,250,"FIRST",0,0);
 b=wm_window_create(&m,400,100,250,200,"SECOND",0,0);
 assert(a && b);
 wm_window_set_close(b,close_request);
 assert(wm_pointer_down(&m,328,61));
 assert(a->closed && !a->visible && m.focused==1);
 assert(wm_hit_test(&m,80,80)==-1);
 assert(wm_pointer_down(&m,628,111));
 assert(!b->closed && b->visible);
 allow=1;
 assert(wm_pointer_down(&m,628,111));
 assert(b->closed && m.focused==-1);
 wm_init(&m,&s,WM_THEME_WIN95);
 a=wm_window_create(&m,50,50,300,250,"FIRST",0,0);
 b=wm_window_create(&m,400,100,250,200,"SECOND",0,0);
 assert(wm_pointer_down(&m,606,111));
 assert(b->maximized && b->x==0 && b->y==0 && b->width==800 && b->height==560);
 assert(wm_pointer_down(&m,756,11));
 assert(!b->maximized && b->x==400 && b->y==100 && b->width==250 && b->height==200);
 assert(wm_pointer_down(&m,584,111));
 assert(b->minimized && !b->visible && m.focused==0);
 assert(wm_window_restore(&m,b));
 assert(b->visible && !b->minimized && m.focused==1);
 wm_init(&m,&s,WM_THEME_MAC);
 a=wm_window_create(&m,50,50,300,250,"MAC",0,0);
 assert(wm_pointer_down(&m,62,60));
 assert(a->closed);
 puts("PASS: WM close veto, focus reassignment, maximize/restore, minimize and Mac controls");
 return 0;
}
