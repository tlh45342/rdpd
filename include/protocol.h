#ifndef RDPD_PROTOCOL_H
#define RDPD_PROTOCOL_H
#include <stdint.h>
#include <stddef.h>
/* Returns 0 on handled request, -1 on malformed/IO error. */
int rdpd_handle_connection(uintptr_t socket_handle);
int rdpd_parse_x224(const unsigned char *packet, size_t length,
                    unsigned int *requested_protocols);
#endif
