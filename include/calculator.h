#ifndef RDPD_CALCULATOR_H
#define RDPD_CALCULATOR_H
#include "gfx.h"
typedef struct calculator {
 long entry,accumulator;
 char op;
 int entering,pending,error;
} calculator;
void calculator_init(calculator *c);
void calculator_key(calculator *c,char key);
int calculator_click(calculator *c,int x,int y);
void calculator_paint(calculator *c,gfx_surface *s);
void calculator_paint_mac(calculator *c,gfx_surface *s);
#endif
