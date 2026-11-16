#include "session.h"
static void calculator_window_click(wm_window *w,int x,int y,void *context)
{
 session *s=(session *)context;
 const wm_theme *t=wm_theme_get(s->manager.theme);
 (void)w;
 (void)calculator_click(&s->calculator_state,x-t->border_width,
                         y-t->border_width-t->title_height);
}
static void calculator_draw(wm_window *w,gfx_surface *surface,void *context)
{
 session *s=(session *)context;
 (void)w;
 calculator_paint(&s->calculator_state,surface);
}
static int launch_calculator(void *context)
{
 session *s=(session *)context;
 wm_window *w=s->calculator_window;
 if(w && !w->closed) {
  if(w->minimized) return wm_window_restore(&s->manager,w);
  return wm_raise(&s->manager,w);
 }
 calculator_init(&s->calculator_state);
 w=wm_window_create(&s->manager,660,235,300,345,
                    "CALCULATOR",calculator_window_click,s);
 if(!w) return 0;
 wm_window_set_paint(w,calculator_draw);
 s->calculator_window=w;
 return 1;
}
void session_init(session *s,gfx_surface *surface,wm_theme_id theme)
{
 wm_init(&s->manager,surface,theme);
 app_registry_init(&s->apps);
 s->calculator_window=0;
 calculator_init(&s->calculator_state);
 (void)app_register(&s->apps,"calculator",launch_calculator,s);
}
int session_launch(session *s,const char *id)
{
 return app_launch(&s->apps,id);
}
void session_draw(session *s) {wm_draw(&s->manager);}
int session_pointer(session *s,int x,int y,int kind)
{
 if(kind==1) return wm_pointer_down(&s->manager,x,y);
 if(kind==2) return wm_pointer_move(&s->manager,x,y);
 if(kind==3) wm_pointer_up(&s->manager);
 return 0;
}
