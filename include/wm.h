#ifndef WM_H
#define WM_H
#include "gfx.h"
#define WM_MAX_WINDOWS 16
typedef enum wm_theme_id {
 WM_THEME_CLASSIC=0, WM_THEME_X11, WM_THEME_WIN95, WM_THEME_WIN10,
 WM_THEME_WIN11, WM_THEME_MAC, WM_THEME_COUNT
} wm_theme_id;
typedef struct wm_theme {
 const char *name;
 unsigned short frame, title, title_text, body, border, control;
 int border_width, title_height, control_count, controls_left;
} wm_theme;
typedef struct wm_window wm_window;
typedef void (*wm_click_fn)(wm_window *,int,int,void *);
typedef void (*wm_paint_fn)(wm_window *,gfx_surface *,void *);
typedef int (*wm_close_fn)(wm_window *,void *);
struct wm_window {
 int x,y,width,height,visible,minimized,maximized,closed;
 int restore_x,restore_y,restore_width,restore_height;
 const char *title;
 wm_click_fn click;
 wm_paint_fn paint;
 wm_close_fn close_request;
 void *userdata;
};
typedef struct wm {
 gfx_surface *surface;
 wm_window windows[WM_MAX_WINDOWS];
 unsigned count;
 unsigned z_order[WM_MAX_WINDOWS];
 int focused;
 int pointer_window, pointer_mode, pointer_x, pointer_y;
 wm_theme_id theme;
} wm;
const wm_theme *wm_theme_get(wm_theme_id id);
int wm_theme_parse(const char *name,wm_theme_id *out);
void wm_init(wm *manager,gfx_surface *surface,wm_theme_id theme);
wm_window *wm_window_create(wm *manager,int x,int y,int width,int height,const char *title,wm_click_fn click,void *userdata);
int wm_hit_test(const wm *manager,int x,int y);
int wm_raise(wm *manager,wm_window *window);
int wm_mouse_down(wm *manager,int x,int y);
int wm_pointer_down(wm *manager,int x,int y);
int wm_pointer_move(wm *manager,int x,int y);
void wm_pointer_up(wm *manager);
void wm_draw_window(wm *manager,const wm_window *window);
void wm_window_set_paint(wm_window *window,wm_paint_fn paint);
void wm_window_set_close(wm_window *window,wm_close_fn callback);
int wm_window_minimize(wm *manager,wm_window *window);
int wm_window_restore(wm *manager,wm_window *window);
int wm_window_toggle_maximize(wm *manager,wm_window *window);
int wm_window_close(wm *manager,wm_window *window);
void wm_draw(wm *manager);
#endif
