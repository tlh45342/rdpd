#include "desktop.h"
#include "gfx.h"
#include "wm.h"
#include <stdlib.h>

/* Application state stays outside WM. Painting uses window-local surfaces. */
static gfx_surface *canvas;
static wm manager;
static wm_window *main_window,*second_window;
static int clicked,second_clicked;

static void main_click(wm_window *w,int x,int y,void *unused)
{
 const wm_theme *t=wm_theme_get(manager.theme);
 int left=t->border_width,top=t->border_width+t->title_height;
 (void)w;(void)unused;
 if(x>=left+85 && x<left+345 && y>=top+105 && y<top+159)
  clicked=!clicked;
}
static void second_click(wm_window *w,int x,int y,void *unused)
{
 const wm_theme *t=wm_theme_get(manager.theme);
 (void)w;(void)unused;
 if(x>=t->border_width+30 && x<t->border_width+190 &&
    y>=t->border_width+t->title_height+75 &&
    y<t->border_width+t->title_height+119)
  second_clicked=!second_clicked;
}
static void main_paint(wm_window *w,gfx_surface *s,void *unused)
{
 (void)w;(void)unused;
 gfx_rect(s,20,25,(int)s->width-40,(int)s->height-50,0xffff);
 gfx_text(s,30,45,"WELCOME TO RDPD",0);
 gfx_text(s,30,75,"TWO WINDOW COMPOSITION",0);
 gfx_rect(s,85,105,260,54,clicked?0x07e0:0x3a8e);
 gfx_text(s,137,125,clicked?"CLICKED":"CLICK ME",0xffff);
}
static void second_paint(wm_window *w,gfx_surface *s,void *unused)
{
 (void)w;(void)unused;
 gfx_rect(s,12,15,(int)s->width-24,(int)s->height-30,0xffff);
 gfx_text(s,22,30,"SECOND WINDOW",0);
 gfx_rect(s,30,75,160,44,second_clicked?0xf800:0x2190);
 gfx_text(s,48,90,second_clicked?"ACTIVE":"PRESS ME",0xffff);
}
void rdpd_desktop_set_surface(gfx_surface *surface)
{
 const char *requested=getenv("RDPD_THEME");
 wm_theme_id theme=WM_THEME_CLASSIC;
 if(requested) (void)wm_theme_parse(requested,&theme);
 canvas=surface;clicked=0;second_clicked=0;
 wm_init(&manager,surface,theme);
 main_window=wm_window_create(&manager,75,70,530,395,
                              "RDPD DESKTOP",main_click,0);
 second_window=wm_window_create(&manager,355,205,360,275,
                                "SECOND WINDOW",second_click,0);
 wm_window_set_paint(main_window,main_paint);
 wm_window_set_paint(second_window,second_paint);
}
void rdpd_desktop_draw(void)
{
 if(!canvas || !main_window || !second_window) return;
 gfx_clear(canvas,0x2190);
 gfx_rect(canvas,0,560,800,40,0x1082);
 gfx_rect(canvas,12,566,95,27,0x4a69);
 gfx_text(canvas,23,572,"START",0xffff);
 wm_draw(&manager);
 gfx_text(canvas,150,570,"RDPD 0.0.1",0xffff);
 if(main_window->minimized)
  gfx_text(canvas,320,572,"RESTORE MAIN",0xffff);
 if(second_window->minimized)
  gfx_text(canvas,550,572,"RESTORE SECOND",0xffff);
}
int rdpd_desktop_pointer(unsigned x,unsigned y,int kind)
{
 int before=clicked,second_before=second_clicked,changed=0;
 if(kind==1) {
  if(y>=560 && main_window->minimized && x>=310 && x<520)
   changed=wm_window_restore(&manager,main_window);
  else if(y>=560 && second_window->minimized && x>=540 && x<790)
   changed=wm_window_restore(&manager,second_window);
  else changed=wm_pointer_down(&manager,(int)x,(int)y);
 }
 else if(kind==2) changed=wm_pointer_move(&manager,(int)x,(int)y);
 else if(kind==3) wm_pointer_up(&manager);
 if(changed || clicked!=before || second_clicked!=second_before) {
  rdpd_desktop_draw();return 1;
 }
 return 0;
}
int rdpd_desktop_click(unsigned x,unsigned y)
{
 return rdpd_desktop_pointer(x,y,1);
}
