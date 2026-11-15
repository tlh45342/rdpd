#ifndef RDPD_CLIENT_INFO_H
#define RDPD_CLIENT_INFO_H
#include <stddef.h>
/* Returns 1 for accepted development credentials, 0 for rejected,
   -1 for malformed Client Info. Never logs credential contents. */
int rdpd_parse_client_info(const unsigned char *p, size_t n);
#endif
