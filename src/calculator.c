#include "calculator.h"
#include "widget.h"
#include <stdio.h>
#include <string.h>
static const char *keys[4][4]={
 {"7","8","9","/"},{"4","5","6","*"},{"1","2","3","-"},{"C","0","=","+"}
};
void calculator_init(calculator *c) { memset(c,0,sizeof(*c)); c->op='+'; }
static void apply(calculator *c)
{
 long b=c->entry,a=c->accumulator;
 if(c->op=='+') a+=b;
 else if(c->op=='-') a-=b;
 else if(c->op=='*') a*=b;
 else if(c->op=='/') {if(!b) {c->error=1;return;} a/=b;}
 c->accumulator=a;c->entry=a;c->entering=0;
}
void calculator_key(calculator *c,char key)
{
 if(key=='C') {calculator_init(c);return;}
 if(key>='0' && key<='9') {
  if(!c->entering) {c->entry=0;c->entering=1;}
  if(c->entry<10000000L) c->entry=c->entry*10+(key-'0');
  c->error=0;return;
 }
 if(key=='+' || key=='-' || key=='*' || key=='/') {
  if(c->pending && c->entering) {apply(c);if(c->error)return;}
  else if(!c->pending) c->accumulator=c->entry;
  c->op=key;c->pending=1;c->entering=0;return;
 }
 if(key=='=' && c->pending) {
  apply(c);c->pending=0;
 }
}
int calculator_click(calculator *c,int x,int y)
{
 int col,row;
 if(x<20 || y<85 || x>=260 || y>=285) return 0;
 col=(x-20)/60;row=(y-85)/50;
 calculator_key(c,keys[row][col][0]);return 1;
}
static void calculator_paint_style(calculator *c,gfx_surface *s,int mac)
{
 int r,k;
 char display[40];
 gfx_rect(s,12,12,256,58,0xffff);
 if(c->error) strcpy(display,"DIV BY ZERO");
 else snprintf(display,sizeof(display),"%ld",c->entry);
 gfx_text(s,24,30,display,0x0000);
 for(r=0;r<4;r++) for(k=0;k<4;k++) {
  widget b=widget_button(20+k*60,85+r*50,56,45,keys[r][k],0,0x39e7,0);
  if(mac) widget_draw_mac(&b,s,0);
  else widget_draw(&b,s,0);
 }
}

void calculator_paint(calculator *c,gfx_surface *s)
{ calculator_paint_style(c,s,0); }
void calculator_paint_mac(calculator *c,gfx_surface *s)
{ calculator_paint_style(c,s,1); }
