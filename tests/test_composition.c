#include "wm.h"
#include <assert.h>
#include <stdio.h>
static void paint(wm_window *w,gfx_surface *s,void *userdata)
{
 (void)w;
 gfx_clear(s,*(uint16_t *)userdata);
}
int main(void)
{
 uint32_t pixels[160*120];
 uint16_t green=0x07e0,red=0xf800;
 gfx_surface s={pixels,160,120,160};
 wm m;
 wm_window *a,*b;
 wm_init(&m,&s,WM_THEME_WIN95);
 a=wm_window_create(&m,5,5,100,100,"BACK",0,&green);
 b=wm_window_create(&m,35,25,100,90,"FRONT",0,&red);
 assert(a && b);
 wm_window_set_paint(a,paint);
 wm_window_set_paint(b,paint);
 gfx_clear(&s,0);
 wm_draw(&m);
 assert(pixels[70*160+50]==gfx_from_rgb565(0xf800));
 assert(pixels[70*160+20]==gfx_from_rgb565(0x07e0));
 assert(wm_raise(&m,a));
 gfx_clear(&s,0);
 wm_draw(&m);
 assert(pixels[70*160+50]==gfx_from_rgb565(0x07e0));
 assert(pixels[70*160+115]==gfx_from_rgb565(0xf800));
 a->x=-15;
 gfx_clear(&s,0);
 wm_draw(&m);
 assert(pixels[70*160+5]==gfx_from_rgb565(0x07e0));
 puts("PASS: independent window surfaces, occlusion, raise and screen clipping");
 return 0;
}
