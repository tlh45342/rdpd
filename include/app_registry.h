#ifndef RDPD_APP_REGISTRY_H
#define RDPD_APP_REGISTRY_H
#define APP_MAX 8
typedef int (*app_launch_fn)(void *);
typedef struct app_entry {
 const char *id;
 app_launch_fn launch;
 void *context;
} app_entry;
typedef struct app_registry {
 app_entry entries[APP_MAX];
 unsigned count;
} app_registry;
void app_registry_init(app_registry *r);
int app_register(app_registry *r,const char *id,app_launch_fn launch,void *context);
int app_launch(app_registry *r,const char *id);
#endif
