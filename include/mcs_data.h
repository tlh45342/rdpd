#ifndef RDPD_MCS_DATA_H
#define RDPD_MCS_DATA_H
#include <stddef.h>
/* Decode MCS SendDataRequest (0x64) and inspect RDP security flags.
   Returns 0 on success, -1 on malformed or unsupported framing. */
int rdpd_inspect_send_data(const unsigned char *p, size_t n);
#endif
