#include "mac_icons.h"
#include "../resources/icons/mac_icon_assets.inc"
/* Palette index 0 is transparent; all images are 48x48. */
void mac_icon_draw(gfx_surface *s,int x,int y,mac_icon_id id)
{
 const icon_resource *resource=id==MAC_ICON_DISK?&mac_disk:
  id==MAC_ICON_TRASH?&mac_trash:&mac_calculator;
 int row,col;
 if(!s || !s->pixels) return;
 for(row=0;row<48;row++) {
  int yy=y+row;
  if(yy<0 || (unsigned)yy>=s->height) continue;
  for(col=0;col<48;col++) {
   int xx=x+col;
   char c=resource->rows[row][col];
   unsigned index=(unsigned)(c<='9'?c-'0':c-'a'+10);
   if(!index || index>15 || xx<0 || (unsigned)xx>=s->width)continue;
   s->pixels[(size_t)yy*s->stride+(unsigned)xx]=resource->palette[index];
  }
 }
}
