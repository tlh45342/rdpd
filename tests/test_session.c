#include "session.h"
#include <assert.h>
#include <stdio.h>
static uint16_t pixels[1024*768];
int main(void)
{
 gfx_surface surface={pixels,1024,768,1024};
 session s;
 wm_window *first,*second;
 unsigned i;
 session_init(&s,&surface,WM_THEME_WIN95);
 assert(session_launch(&s,"calculator"));
 first=s.calculator_window;
 assert(first && first->visible && s.manager.count==1);
 assert(session_launch(&s,"calculator"));
 assert(s.calculator_window==first && s.manager.count==1);
 assert(wm_window_minimize(&s.manager,first));
 assert(session_launch(&s,"calculator") && first->visible);
 assert(wm_window_close(&s.manager,first));
 assert(session_launch(&s,"calculator"));
 second=s.calculator_window;
 assert(second==first && s.manager.count==1);
 for(i=0;i<100;i++) {
  assert(wm_window_close(&s.manager,s.calculator_window));
  assert(session_launch(&s,"calculator"));
  assert(s.manager.count==1);
 }
 assert(!session_launch(&s,"not-registered"));
 assert(session_pointer(&s,700,360,1));
 assert(s.calculator_state.entry==7);
 session_draw(&s);
 puts("PASS: session launch, raise, minimize, close, 100 slot reuses and calculator pointer routing");
 return 0;
}
