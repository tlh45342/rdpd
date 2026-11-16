#ifndef RDPD_LOG_H
#define RDPD_LOG_H
#include <stdint.h>
void rdpd_log_init(void);
void rdpd_log_close(void);
void rdpd_log_event(const char *level,uint64_t session,const char *event,const char *fmt,...);
#endif
