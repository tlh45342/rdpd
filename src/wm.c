#include "wm.h"
#include <string.h>
#include <stdlib.h>
/* Theme values are RGB565. Frame layout is data, never application logic. */
static const wm_theme themes[WM_THEME_COUNT]={
 {"classic",0x0000,0x19a7,0xffff,0xce79,0x0000,0xce79,4,38,1,0},
 {"x11",0x39e7,0x2945,0xffff,0xd69a,0x0000,0xbdf7,3,25,3,0},
 {"win95",0xc618,0x0010,0xffff,0xc618,0x7bef,0xc618,4,27,3,0},
 {"win10",0xf7de,0x2190,0xffff,0xffff,0x9cf3,0xf7de,1,32,3,0},
 {"win11",0xf7de,0xdefb,0x2104,0xffff,0xc638,0xf7de,2,36,3,0},
 {"mac",0xe71c,0xe71c,0x2104,0xffff,0xad55,0xf800,2,32,3,1}
};
const wm_theme *wm_theme_get(wm_theme_id id)
{
 if ((unsigned)id>=WM_THEME_COUNT) return &themes[WM_THEME_CLASSIC];
 return &themes[id];
}
int wm_theme_parse(const char *name,wm_theme_id *out)
{
 unsigned i;
 if (!name || !out) return 0;
 for(i=0;i<WM_THEME_COUNT;i++)
  if(strcmp(name,themes[i].name)==0) {*out=(wm_theme_id)i;return 1;}
 return 0;
}
void wm_init(wm *m,gfx_surface *surface,wm_theme_id theme)
{
 if(!m) return;
 memset(m,0,sizeof(*m));
 m->surface=surface;
 m->theme=(unsigned)theme<WM_THEME_COUNT?theme:WM_THEME_CLASSIC;
 m->focused=-1;
 m->pointer_window=-1;
}
wm_window *wm_window_create(wm *m,int x,int y,int width,int height,const char *title,wm_click_fn click,void *userdata)
{
 wm_window *w;
 if(!m || m->count>=WM_MAX_WINDOWS || width<=0 || height<=0) return 0;
 w=&m->windows[m->count];
 m->z_order[m->count]=m->count;
 m->count++;
 w->x=x;w->y=y;w->width=width;w->height=height;
 w->title=title;w->click=click;w->userdata=userdata;w->visible=1;
 m->focused=(int)m->count-1;
 return w;
}
int wm_hit_test(const wm *m,int x,int y)
{
 int i;
 if(!m) return -1;
 for(i=(int)m->count-1;i>=0;i--) {
  const wm_window *w=&m->windows[m->z_order[i]];
  if(w->visible && !w->closed && x>=w->x && y>=w->y &&
     x-w->x<w->width && y-w->y<w->height) return (int)m->z_order[i];
 }
 return -1;
}
/* Stable window handles: only the z-order index array moves. */
int wm_raise(wm *m,wm_window *w)
{
 unsigned i,j,index;
 if(!m || !w) return 0;
 for(index=0;index<m->count;index++)
  if(&m->windows[index]==w) break;
 if(index==m->count || !w->visible || w->closed) return 0;
 for(i=0;i<m->count;i++) if(m->z_order[i]==index) break;
 if(i==m->count) return 0;
 for(j=i;j+1<m->count;j++) m->z_order[j]=m->z_order[j+1];
 m->z_order[m->count-1]=index;
 m->focused=(int)index;
 return 1;
}
static void wm_refocus(wm *m)
{
 int i;
 m->focused=-1;
 for(i=(int)m->count-1;i>=0;i--) {
  unsigned slot=m->z_order[i];
  if(m->windows[slot].visible && !m->windows[slot].closed) {
   m->focused=(int)slot;break;
  }
 }
}
void wm_window_set_close(wm_window *w,wm_close_fn callback)
{
 if(w) w->close_request=callback;
}
int wm_window_minimize(wm *m,wm_window *w)
{
 if(!m || !w || w->closed || !w->visible) return 0;
 w->minimized=1;w->visible=0;
 if(m->pointer_window==(int)(w-m->windows)) wm_pointer_up(m);
 wm_refocus(m);
 return 1;
}
int wm_window_restore(wm *m,wm_window *w)
{
 if(!m || !w || w->closed || (!w->minimized && !w->maximized)) return 0;
 if(w->maximized) {
  w->x=w->restore_x;w->y=w->restore_y;
  w->width=w->restore_width;w->height=w->restore_height;
  w->maximized=0;
 }
 w->minimized=0;w->visible=1;
 (void)wm_raise(m,w);
 return 1;
}
int wm_window_toggle_maximize(wm *m,wm_window *w)
{
 if(!m || !m->surface || !w || w->closed || !w->visible) return 0;
 if(w->maximized) return wm_window_restore(m,w);
 w->restore_x=w->x;w->restore_y=w->y;
 w->restore_width=w->width;w->restore_height=w->height;
 w->x=0;w->y=0;w->width=(int)m->surface->width;
 w->height=(int)m->surface->height-40;
 w->maximized=1;
 (void)wm_raise(m,w);
 return 1;
}
int wm_window_close(wm *m,wm_window *w)
{
 if(!m || !w || w->closed) return 0;
 /* A callback can veto close; 0 means the application keeps its window. */
 if(w->close_request && !w->close_request(w,w->userdata)) return 0;
 w->closed=1;w->visible=0;w->minimized=0;
 if(m->pointer_window==(int)(w-m->windows)) wm_pointer_up(m);
 wm_refocus(m);
 return 1;
}
static int wm_control(wm *m,wm_window *w,int x,int y)
{
 const wm_theme *t=wm_theme_get(m->theme);
 int i,cy=w->y+t->border_width+7;
 if(y<cy || y>=cy+14) return 0;
 for(i=0;i<t->control_count;i++) {
  int cx=t->controls_left?w->x+12+i*22:w->x+w->width-22-i*22;
  if(x<cx || x>=cx+14) continue;
  if(t->controls_left) {
   if(i==0) (void)wm_window_close(m,w);
   else if(i==1) (void)wm_window_minimize(m,w);
   else (void)wm_window_toggle_maximize(m,w);
  } else {
   if(i==0) (void)wm_window_close(m,w);
   else if(i==1) (void)wm_window_toggle_maximize(m,w);
   else (void)wm_window_minimize(m,w);
  }
  return 1;
 }
 return 0;
}
int wm_mouse_down(wm *m,int x,int y)
{
 int i=wm_hit_test(m,x,y);
 wm_window *w;
 if(i<0) return 0;
 m->focused=i;
 w=&m->windows[i];
 (void)wm_raise(m,w);
 if(wm_control(m,w,x,y)) return 1;
 if(w->click) w->click(w,x-w->x,y-w->y,w->userdata);
 return 1;
}
/* Pointer capture: title bar drags, bottom-right grip resizes.
   All coordinates are screen-relative; applications receive local clicks. */
int wm_pointer_down(wm *m,int x,int y)
{
 int i;
 wm_window *w;
 const wm_theme *t;
 if(!m) return 0;
 i=wm_hit_test(m,x,y);
 m->pointer_window=-1;m->pointer_mode=0;
 if(i<0) return 0;
 w=&m->windows[i];t=wm_theme_get(m->theme);
 m->focused=i;
 (void)wm_raise(m,w);
 if(wm_control(m,w,x,y)) return 1;
 if(x>=w->x+w->width-12 && y>=w->y+w->height-12) {
  m->pointer_mode=2;
 } else if(y<w->y+t->border_width+t->title_height) {
  m->pointer_mode=1;
 } else {
  if(w->click) w->click(w,x-w->x,y-w->y,w->userdata);
  return 1;
 }
 m->pointer_window=i;m->pointer_x=x;m->pointer_y=y;
 return 1;
}
int wm_pointer_move(wm *m,int x,int y)
{
 wm_window *w;
 int dx,dy;
 if(!m || m->pointer_window<0 || !m->pointer_mode) return 0;
 w=&m->windows[m->pointer_window];
 dx=x-m->pointer_x;dy=y-m->pointer_y;
 if(!dx && !dy) return 0;
 if(w->maximized) return 0;
 if(m->pointer_mode==1) {
  w->x+=dx;w->y+=dy;
 } else {
  int nw=w->width+dx,nh=w->height+dy;
  if(nw<120) nw=120;
  if(nh<80) nh=80;
  w->width=nw;w->height=nh;
 }
 m->pointer_x=x;m->pointer_y=y;
 return 1;
}
void wm_pointer_up(wm *m)
{
 if(!m) return;
 m->pointer_window=-1;m->pointer_mode=0;
}
void wm_window_set_paint(wm_window *w,wm_paint_fn paint)
{
 if(w) w->paint=paint;
}
void wm_draw_window(wm *m,const wm_window *w)
{
 const wm_theme *t;
 int i,cy,control_x;
 if(!m || !m->surface || !w || !w->visible || w->closed) return;
 t=wm_theme_get(m->theme);
 gfx_rect(m->surface,w->x,w->y,w->width,w->height,t->frame);
 gfx_rect(m->surface,w->x+t->border_width,w->y+t->border_width,
  w->width-2*t->border_width,w->height-2*t->border_width,t->body);
 gfx_rect(m->surface,w->x+t->border_width,w->y+t->border_width,
  w->width-2*t->border_width,t->title_height,t->title);
 gfx_text(m->surface,w->x+t->border_width+(t->controls_left?85:15),w->y+t->border_width+10,w->title,t->title_text);
 cy=w->y+t->border_width+7;
 for(i=0;i<t->control_count;i++) {
  control_x=t->controls_left?w->x+12+i*22:w->x+w->width-22-i*22;
  gfx_rect(m->surface,control_x,cy,14,14,
   t->controls_left?(i==0?0xf800:(i==1?0xffc0:0x07e0)):t->control);
 }
 /* Paint application content into a window-local surface, then composite
    only the client rectangle. This prevents content leaking over frames
    or into neighboring windows, and respects the z-order draw sequence. */
 if(w->paint && w->width>2*t->border_width &&
    w->height>t->title_height+2*t->border_width) {
  int left=t->border_width,top=t->border_width+t->title_height;
  int cw=w->width-2*left,ch=w->height-top-t->border_width;
  size_t count=(size_t)cw*(size_t)ch;
  uint16_t *pixels;
  gfx_surface client;
  int row,col;
  if(cw>4096 || ch>4096 || count>1048576) return;
  pixels=(uint16_t *)malloc(count*sizeof(*pixels));
  if(!pixels) return;
  client.pixels=pixels;client.width=(unsigned)cw;
  client.height=(unsigned)ch;client.stride=(unsigned)cw;
  gfx_clear(&client,t->body);
  w->paint((wm_window *)w,&client,w->userdata);
  for(row=0;row<ch;row++) {
   int dy=w->y+top+row;
   if(dy<0 || (unsigned)dy>=m->surface->height) continue;
   for(col=0;col<cw;col++) {
    int dx=w->x+left+col;
    if(dx>=0 && (unsigned)dx<m->surface->width)
     m->surface->pixels[(size_t)dy*m->surface->stride+(unsigned)dx]=pixels[(size_t)row*cw+(unsigned)col];
   }
  }
  free(pixels);
 }
}
void wm_draw(wm *m)
{
 unsigned i;
 if(!m) return;
 for(i=0;i<m->count;i++) wm_draw_window(m,&m->windows[m->z_order[i]]);
}
