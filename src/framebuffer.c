#include "framebuffer.h"
#include "desktop.h"
#include "gfx.h"
#include <string.h>

/* Synthetic desktop: a 32-bit RGB888 desktop framebuffer. */
static uint32_t framebuffer[RDPD_WIDTH * RDPD_HEIGHT];
static int initialized;

static void w16(unsigned char *p, unsigned v)
{
    p[0]=(unsigned char)v;
    p[1]=(unsigned char)(v>>8);
}
static void w32(unsigned char *p, unsigned long v)
{
    w16(p,(unsigned)v);
    w16(p+2,(unsigned)(v>>16));
}
static gfx_surface canvas={framebuffer,RDPD_WIDTH,RDPD_HEIGHT,RDPD_WIDTH};
void rdpd_framebuffer_clear(unsigned short color)
{
    gfx_clear(&canvas,color);
    initialized=1;
}
void rdpd_framebuffer_rect(int x,int y,int width,int height,unsigned short color)
{
    gfx_rect(&canvas,x,y,width,height,color);
    initialized=1;
}
static void init_framebuffer(void)
{
    if (initialized) return;
    rdpd_desktop_set_surface(&canvas);
    rdpd_desktop_draw();
    initialized=1;
}
int rdpd_build_bitmap_tile(unsigned index, unsigned char *out,
                           size_t cap, size_t *used)
{
    unsigned x,y,w,h,row,col;
    size_t pixels,share_len,user_len,total,pos;
    if (!out || !used || index>=RDPD_TILE_COUNT) return -1;
    init_framebuffer();
    x=(index%RDPD_TILES_X)*RDPD_TILE_WIDTH;
    y=(index/RDPD_TILES_X)*RDPD_TILE_HEIGHT;
    w=RDPD_WIDTH-x<RDPD_TILE_WIDTH?RDPD_WIDTH-x:RDPD_TILE_WIDTH;
    h=RDPD_HEIGHT-y<RDPD_TILE_HEIGHT?RDPD_HEIGHT-y:RDPD_TILE_HEIGHT;
    pixels=(size_t)w*h*4;
    /* Share Data header (18), Update (4), Bitmap rectangle (18). */
    share_len=18+4+18+pixels;
    user_len=share_len;
    total=7+6+2+user_len;
    if (user_len>0x7fff || cap<total || total>65535) return -1;
    memset(out,0,total);
    out[0]=3;w16(out+2,0); /* TPKT length is big endian below */
    out[2]=(unsigned char)(total>>8);
    out[3]=(unsigned char)total;
    out[4]=2;out[5]=0xf0;out[6]=0x80;
    out[7]=0x68;out[8]=0;out[9]=1;out[10]=3;out[11]=0xeb;out[12]=0x70;
    out[13]=(unsigned char)(0x80|(user_len>>8));
    out[14]=(unsigned char)user_len;
    pos=15;
    w16(out+pos,(unsigned)share_len);
    w16(out+pos+2,0x0017);
    w16(out+pos+4,1002);
    w32(out+pos+6,0x000103ea);
    out[pos+10]=0;
    out[pos+11]=1;
    w16(out+pos+12,(unsigned)share_len);
    out[pos+14]=2; /* PDUTYPE2_UPDATE */
    pos+=18;
    w16(out+pos,1); /* UPDATETYPE_BITMAP */
    w16(out+pos+2,1); /* numberRectangles */
    pos+=4;
    w16(out+pos,x);w16(out+pos+2,y);
    w16(out+pos+4,x+w-1);w16(out+pos+6,y+h-1);
    w16(out+pos+8,w);w16(out+pos+10,h);
    w16(out+pos+12,32); /* bitsPerPixel */
    w16(out+pos+14,0); /* uncompressed */
    w16(out+pos+16,(unsigned)pixels);
    pos+=18;
    /* RDP uncompressed bitmap scanlines are bottom-up. */
    for (row=0;row<h;row++)
        for (col=0;col<w;col++)
            w32(out+pos+((size_t)row*w+col)*4,
                framebuffer[(size_t)(y+h-1-row)*RDPD_WIDTH+x+col]);
    *used=total;
    return 0;
}
