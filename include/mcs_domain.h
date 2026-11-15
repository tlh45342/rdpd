#ifndef RDPD_MCS_DOMAIN_H
#define RDPD_MCS_DOMAIN_H
#include <stddef.h>
/* Return: 1 response built, 0 valid request without response, -1 invalid. */
int rdpd_mcs_domain_step(const unsigned char *packet, size_t length,
                         unsigned char *response, size_t capacity,
                         size_t *response_length, int *state);
#endif
