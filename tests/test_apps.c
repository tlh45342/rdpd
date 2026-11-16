#include "calculator.h"
#include "app_registry.h"
#include <assert.h>
#include <stdio.h>
static int launches;
static int launch(void *ctx) {(*(int *)ctx)++;return 1;}
int main(void)
{
 calculator c;
 app_registry r;
 uint16_t pixels[300*300];
 gfx_surface s={pixels,300,300,300};
 calculator_init(&c);
 calculator_key(&c,'1');calculator_key(&c,'2');
 calculator_key(&c,'+');calculator_key(&c,'3');calculator_key(&c,'=');
 assert(c.entry==15 && !c.error);
 calculator_key(&c,'C');assert(c.entry==0);
 calculator_key(&c,'8');calculator_key(&c,'/');calculator_key(&c,'0');calculator_key(&c,'=');
 assert(c.error);
 calculator_key(&c,'C');assert(!c.error);
 assert(calculator_click(&c,30,95));assert(c.entry==7);
 assert(!calculator_click(&c,300,300));
 calculator_paint(&c,&s);
 app_registry_init(&r);
 assert(app_register(&r,"calculator",launch,&launches));
 assert(!app_register(&r,"calculator",launch,&launches));
 assert(app_launch(&r,"calculator") && launches==1);
 assert(!app_launch(&r,"unknown"));
 puts("PASS: calculator arithmetic, zero division, click geometry, app registration and launch");
 return 0;
}
