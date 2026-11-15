#ifndef RDPD_ACTIVATION_H
#define RDPD_ACTIVATION_H
#include <stddef.h>
/* Build TPKT/X224/MCS SendDataIndication containing Server Demand Active. */
int rdpd_build_demand_active(unsigned char *out, size_t cap, size_t *used);
/* Inspect a client Confirm Active carried in an MCS SendDataRequest. */
int rdpd_inspect_confirm_active(const unsigned char *p, size_t n);
#endif
