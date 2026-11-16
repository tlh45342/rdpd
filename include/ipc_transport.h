#ifndef RDPD_IPC_TRANSPORT_H
#define RDPD_IPC_TRANSPORT_H
#include "ipc.h"
#include <stddef.h>
/* One framed message per call; return 0 on success, -1 on I/O/EOF,
   -2 on invalid frame. No network or RDP dependency. */
int ipc_send_fd(int fd,const ipc_message *message);
int ipc_recv_fd(int fd,ipc_message *message,uint8_t *storage,size_t capacity);
#endif
