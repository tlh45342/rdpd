#ifndef GFX_H
#define GFX_H
#include <stddef.h>
#include <stdint.h>
typedef struct gfx_surface {
    uint16_t *pixels;
    unsigned width, height, stride;
} gfx_surface;
void gfx_clear(gfx_surface *s, uint16_t color);
void gfx_rect(gfx_surface *s, int x, int y, int w, int h, uint16_t color);
void gfx_text(gfx_surface *s, int x, int y, const char *text, uint16_t color);
#endif
