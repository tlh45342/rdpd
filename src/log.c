#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
static FILE *destination;
static int owns_file;
void rdpd_log_init(void)
{
 const char *path=getenv("RDPD_LOG_FILE");
 if(destination)return;
 if(!path || !*path) {
#ifdef _WIN32
  path="rdpd.log";
#else
  path="/var/log/rdpd.log";
#endif
 }
 destination=fopen(path,"a");
 if(!destination) {
  fprintf(stderr,"rdpd: cannot open log %s; using stderr\n",path);
  destination=stderr;
 } else owns_file=1;
}
void rdpd_log_close(void)
{
 if(owns_file && destination)fclose(destination);
 destination=0;owns_file=0;
}
void rdpd_log_event(const char *level,uint64_t session,const char *event,const char *fmt,...)
{
 time_t now=time(0);
 struct tm *utc;
 char stamp[32]="unknown";
 va_list args;
 if(!destination)rdpd_log_init();
 utc=gmtime(&now);
 if(utc)strftime(stamp,sizeof(stamp),"%Y-%m-%dT%H:%M:%SZ",utc);
 fprintf(destination,"%s level=%s session=%llu event=%s",stamp,
         level?level:"INFO",(unsigned long long)session,event?event:"unknown");
 if(fmt && *fmt) {
  fputc(' ',destination);
  va_start(args,fmt);
  vfprintf(destination,fmt,args);
  va_end(args);
 }
 fputc('\n',destination);
 fflush(destination);
}
