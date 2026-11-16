#include "app_registry.h"
#include <string.h>
void app_registry_init(app_registry *r) {r->count=0;}
int app_register(app_registry *r,const char *id,app_launch_fn launch,void *context)
{
 unsigned i;
 if(!r || !id || !launch || r->count==APP_MAX)return 0;
 for(i=0;i<r->count;i++) if(!strcmp(r->entries[i].id,id))return 0;
 r->entries[r->count].id=id;
 r->entries[r->count].launch=launch;
 r->entries[r->count].context=context;
 r->count++;return 1;
}
int app_launch(app_registry *r,const char *id)
{
 unsigned i;
 if(!r || !id)return 0;
 for(i=0;i<r->count;i++) if(!strcmp(r->entries[i].id,id))
  return r->entries[i].launch(r->entries[i].context);
 return 0;
}
