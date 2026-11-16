#ifndef GFX_H
#define GFX_H
#include <stddef.h>
#include <stdint.h>
typedef struct gfx_surface {
    uint32_t *pixels;
    unsigned width, height, stride;
} gfx_surface;
/* Surfaces store 0x00RRGGBB (32 bits per pixel). */
uint32_t gfx_from_rgb565(uint16_t value);
uint32_t gfx_blend_rgba(uint32_t destination,uint32_t rgba);
void gfx_rect_rgb(gfx_surface *s,int x,int y,int w,int h,uint32_t rgb);
void gfx_clear(gfx_surface *s, uint16_t color);
void gfx_rect(gfx_surface *s, int x, int y, int w, int h, uint16_t color);
void gfx_text(gfx_surface *s, int x, int y, const char *text, uint16_t color);
#endif
