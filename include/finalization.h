#ifndef RDPD_FINALIZATION_H
#define RDPD_FINALIZATION_H
#include <stddef.h>
/* Decode client Share Data PDUs; 1=font list, 0=other valid, -1=invalid. */
int rdpd_inspect_finalization(const unsigned char *packet, size_t length);
/* Build server Synchronize, Control Cooperate, Control Granted, Font Map. */
int rdpd_build_finalization(unsigned index, unsigned char *out, size_t capacity, size_t *used);
#endif
