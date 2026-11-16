#ifndef RDPD_IPC_H
#define RDPD_IPC_H
#include <stddef.h>
#include <stdint.h>
#define IPC_VERSION 1u
#define IPC_HEADER_SIZE 24u
#define IPC_MAX_PAYLOAD 65536u
#define IPC_MAX_FRAME (IPC_HEADER_SIZE+IPC_MAX_PAYLOAD)
enum ipc_type {
 IPC_HELLO=1, IPC_HELLO_ACK=2, IPC_SESSION_OPEN=3, IPC_SESSION_CLOSE=4,
 IPC_POINTER=5, IPC_KEY=6, IPC_APP_LAUNCH=7, IPC_WINDOW_ACTION=8,
 IPC_FRAME_DAMAGE=9, IPC_STATUS=10, IPC_HEARTBEAT=11
};
enum ipc_window_action { IPC_WINDOW_FOCUS=1, IPC_WINDOW_MINIMIZE=2,
 IPC_WINDOW_RESTORE=3, IPC_WINDOW_MAXIMIZE=4, IPC_WINDOW_CLOSE=5 };
enum ipc_error { IPC_OK=0, IPC_INCOMPLETE=1, IPC_INVALID=-1,
 IPC_UNSUPPORTED=-2, IPC_TOO_LARGE=-3, IPC_NO_SPACE=-4 };
typedef struct ipc_message {
 uint16_t type;
 uint32_t session_id;
 uint32_t request_id;
 uint32_t payload_length;
 const uint8_t *payload; /* borrowed: valid while input buffer lives */
} ipc_message;
/* Network byte order; header: magic(4), version(2), type(2),
   session(4), request(4), payload_length(4), reserved(4). */
int ipc_encode(uint8_t *out,size_t capacity,const ipc_message *msg,size_t *written);
int ipc_decode(const uint8_t *bytes,size_t length,ipc_message *msg,size_t *consumed);
int ipc_frame_length(const uint8_t *bytes,size_t length,size_t *frame_length);
void ipc_put_u32(uint8_t *p,uint32_t value);
uint32_t ipc_get_u32(const uint8_t *p);
#endif
