#ifndef RDPD_MCS_RESPONSE_H
#define RDPD_MCS_RESPONSE_H
#include <stddef.h>
int rdpd_build_mcs_response(unsigned char *out, size_t cap, size_t *used);
#endif
