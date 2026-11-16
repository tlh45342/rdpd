#ifndef RDPD_SESSION_H
#define RDPD_SESSION_H
#include "wm.h"
#include "calculator.h"
#include "app_registry.h"
typedef struct session {
 wm manager;
 app_registry apps;
 wm_window *calculator_window;
 calculator calculator_state;
} session;
void session_init(session *s,gfx_surface *surface,wm_theme_id theme);
int session_launch(session *s,const char *id);
void session_draw(session *s);
int session_pointer(session *s,int x,int y,int kind);
#endif
