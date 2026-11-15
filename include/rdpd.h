#ifndef RDPD_H
#define RDPD_H
#include <signal.h>
#define RDPD_DEFAULT_PORT 3389
int rdpd_serve(unsigned short port, volatile sig_atomic_t *running);
#endif
