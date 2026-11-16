#include "log.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
 char buffer[1024];
 FILE *f;
#ifdef _WIN32
 _putenv("RDPD_LOG_FILE=rdpd-test.log");
#else
 setenv("RDPD_LOG_FILE","rdpd-test.log",1);
#endif
 rdpd_log_init();
 rdpd_log_event("INFO",42,"session_active","bpp=%d",32);
 rdpd_log_close();
 f=fopen("rdpd-test.log","r");
 assert(f);
 assert(fgets(buffer,sizeof(buffer),f));
 assert(strstr(buffer,"level=INFO session=42 event=session_active bpp=32"));
 fclose(f);
 remove("rdpd-test.log");
 puts("PASS: timestamped session event logging");
 return 0;
}
