#include "desktop.h"
#include "gfx.h"
#include "wm.h"
#include "widget.h"
#include "icon.h"
#include "framebuffer.h"
#include "session.h"
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
#include <stdlib.h>

/* Application state stays outside WM. Painting uses window-local surfaces. */
static gfx_surface *canvas;
static session current_session;
#define manager current_session.manager
static wm_window *main_window,*second_window;
static int clicked,second_clicked;
static widget main_button,second_button;
static int icon_selected;
static uint64_t icon_last_click;
static int icon_last_x,icon_last_y;
static uint64_t desktop_time_ms(void)
{
#ifdef _WIN32
 return (uint64_t)GetTickCount64();
#else
 struct timespec now;
 if(clock_gettime(CLOCK_MONOTONIC,&now)!=0) return 0;
 return (uint64_t)now.tv_sec*1000+(uint64_t)now.tv_nsec/1000000;
#endif
}
static void main_click(wm_window *w,int x,int y,void *unused)
{
 const wm_theme *t=wm_theme_get(manager.theme);
 int left=t->border_width,top=t->border_width+t->title_height;
 (void)w;(void)unused;
 if(widget_hit(&main_button,x-left,y-top)) clicked=!clicked;
}
static void second_click(wm_window *w,int x,int y,void *unused)
{
 const wm_theme *t=wm_theme_get(manager.theme);
 (void)w;(void)unused;
 if(widget_hit(&second_button,x-t->border_width,y-t->border_width-t->title_height))
  second_clicked=!second_clicked;
}
static void main_paint(wm_window *w,gfx_surface *s,void *unused)
{
 (void)w;(void)unused;
 gfx_rect(s,20,25,(int)s->width-40,(int)s->height-50,0xffff);
 gfx_text(s,30,45,"WELCOME TO RDPD",0);
 gfx_text(s,30,75,"TWO WINDOW COMPOSITION",0);
 widget_draw(&main_button,s,clicked);
}
static void second_paint(wm_window *w,gfx_surface *s,void *unused)
{
 (void)w;(void)unused;
 gfx_rect(s,12,15,(int)s->width-24,(int)s->height-30,0xffff);
 gfx_text(s,22,30,"SECOND WINDOW",0);
 widget_draw(&second_button,s,second_clicked);
}
void rdpd_desktop_set_surface(gfx_surface *surface)
{
 const char *requested=getenv("RDPD_THEME");
 wm_theme_id theme=WM_THEME_CLASSIC;
 if(requested) (void)wm_theme_parse(requested,&theme);
 canvas=surface;clicked=0;second_clicked=0;
 main_button=widget_button(85,105,260,54,"CLICK ME","CLICKED",0x3a8e,0x07e0);
 second_button=widget_button(30,75,160,44,"PRESS ME","ACTIVE",0x2190,0xf800);
 session_init(&current_session,surface,theme);
 icon_selected=0;icon_last_click=0;
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
 gfx_rect(canvas,0,RDPD_HEIGHT-40,RDPD_WIDTH,40,0x1082);
 gfx_rect(canvas,12,RDPD_HEIGHT-34,95,27,0x4a69);
 gfx_text(canvas,23,RDPD_HEIGHT-28,"START",0xffff);
 if(icon_selected) gfx_rect(canvas,846,91,56,78,0x07ff);
 icon_draw_desktop(canvas,850,95);
 gfx_text(canvas,826,150,"CALCULATOR",0xffff);
 session_draw(&current_session);
 gfx_text(canvas,150,RDPD_HEIGHT-30,"RDPD 0.0.1",0xffff);
 if(main_window->minimized)
  gfx_text(canvas,320,RDPD_HEIGHT-28,"RESTORE MAIN",0xffff);
 if(second_window->minimized)
  gfx_text(canvas,550,RDPD_HEIGHT-28,"RESTORE SECOND",0xffff);
}
int rdpd_desktop_pointer_at(unsigned x,unsigned y,int kind,uint64_t now_ms)
{
 int before=clicked,second_before=second_clicked,changed=0;
 if(kind==1) {
  /* Timed double-click: same icon, <=500ms, <=8px displacement. */
  if(x>=826 && x<925 && y>=91 && y<171 && wm_hit_test(&manager,(int)x,(int)y)<0) {
   if(icon_selected && now_ms>=icon_last_click &&
      now_ms-icon_last_click<=500 &&
      abs((int)x-icon_last_x)<=8 && abs((int)y-icon_last_y)<=8) {
    changed=session_launch(&current_session,"calculator");
    icon_selected=0;
   } else {
    icon_selected=1;icon_last_click=now_ms;
    icon_last_x=(int)x;icon_last_y=(int)y;changed=1;
   }
  } else if(y>=RDPD_HEIGHT-40 && main_window->minimized && x>=310 && x<520)
   changed=wm_window_restore(&manager,main_window);
  else if(y>=RDPD_HEIGHT-40 && second_window->minimized && x>=540 && x<790)
   changed=wm_window_restore(&manager,second_window);
  else {
   if(icon_selected) {icon_selected=0;changed=1;}
   if(session_pointer(&current_session,(int)x,(int)y,kind)) changed=1;
  }
 }
 else if(kind==2) changed=session_pointer(&current_session,(int)x,(int)y,kind);
 else if(kind==3) (void)session_pointer(&current_session,(int)x,(int)y,kind);
 if(changed || clicked!=before || second_clicked!=second_before) {
  rdpd_desktop_draw();return 1;
 }
 return 0;
}
int rdpd_desktop_pointer(unsigned x,unsigned y,int kind)
{
 return rdpd_desktop_pointer_at(x,y,kind,desktop_time_ms());
}
int rdpd_desktop_click(unsigned x,unsigned y)
{
 return rdpd_desktop_pointer(x,y,1);
}
