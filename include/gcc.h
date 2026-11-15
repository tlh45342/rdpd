#ifndef RDPD_GCC_H
#define RDPD_GCC_H
#include <stddef.h>
int rdpd_build_gcc_response(unsigned char *out, size_t cap, size_t *used);
void rdpd_log_gcc_client(const unsigned char *p, size_t n);
#endif
