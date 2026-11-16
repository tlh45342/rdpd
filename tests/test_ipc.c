#include "ipc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void)
{
 uint8_t buffer[IPC_HEADER_SIZE+256],payload[128]={0};
 ipc_message in,out;
 size_t written,used,needed;
 unsigned t;
 for(t=IPC_HELLO;t<=IPC_HEARTBEAT;t++) {
  memset(payload,0,sizeof(payload));
  in.type=(uint16_t)t;in.session_id=0x01020304;in.request_id=0x05060708;
  in.payload=payload;
  switch(t) {
  case IPC_HELLO:case IPC_HELLO_ACK:case IPC_SESSION_OPEN:
  case IPC_KEY:case IPC_STATUS:case IPC_WINDOW_ACTION:in.payload_length=8;break;
  case IPC_POINTER:in.payload_length=12;break;
  case IPC_APP_LAUNCH:memcpy(payload,"calculator",10);in.payload_length=10;break;
  case IPC_FRAME_DAMAGE:in.payload_length=16;break;
  default:in.payload_length=0;
  }
  if(t==IPC_WINDOW_ACTION)ipc_put_u32(payload+4,IPC_WINDOW_CLOSE);
  assert(ipc_encode(buffer,sizeof(buffer),&in,&written)==IPC_OK);
  assert(buffer[0]=='R' && buffer[4]==0 && buffer[5]==1);
  assert(ipc_frame_length(buffer,IPC_HEADER_SIZE,&needed)==
         (in.payload_length?IPC_INCOMPLETE:IPC_OK));
  assert(needed==written);
  assert(ipc_decode(buffer,written,&out,&used)==IPC_OK);
  assert(used==written && out.type==t);
  assert(out.session_id==0x01020304 && out.request_id==0x05060708);
  assert(out.payload_length==in.payload_length);
  assert(!memcmp(out.payload,payload,in.payload_length));
  assert(ipc_decode(buffer,written-1,&out,&used)==IPC_INCOMPLETE);
  assert(ipc_encode(buffer,written-1,&in,&used)==IPC_NO_SPACE);
 }
 in.type=IPC_HEARTBEAT;in.payload_length=0;in.payload=0;
 assert(ipc_encode(buffer,sizeof(buffer),&in,&written)==IPC_OK);
 buffer[0]='X';assert(ipc_decode(buffer,written,&out,&used)==IPC_INVALID);
 buffer[0]='R';buffer[5]=2;
 assert(ipc_decode(buffer,written,&out,&used)==IPC_UNSUPPORTED);
 buffer[5]=1;buffer[7]=99;
 assert(ipc_decode(buffer,written,&out,&used)==IPC_UNSUPPORTED);
 buffer[7]=IPC_HEARTBEAT;buffer[20]=1;
 assert(ipc_decode(buffer,written,&out,&used)==IPC_INVALID);
 buffer[20]=0;ipc_put_u32(buffer+16,IPC_MAX_PAYLOAD+1);
 assert(ipc_decode(buffer,written,&out,&used)==IPC_TOO_LARGE);
 in.type=IPC_APP_LAUNCH;in.payload=(const uint8_t *)"../../etc/passwd";
 in.payload_length=16;
 assert(ipc_encode(buffer,sizeof(buffer),&in,&written)==IPC_INVALID);
 in.type=IPC_WINDOW_ACTION;in.payload=payload;in.payload_length=8;
 ipc_put_u32(payload+4,99);
 assert(ipc_encode(buffer,sizeof(buffer),&in,&written)==IPC_INVALID);
 assert(ipc_decode(buffer,2,&out,&used)==IPC_INCOMPLETE);
 assert(ipc_decode(0,0,&out,&used)==IPC_INVALID ||
        ipc_decode(0,0,&out,&used)==IPC_INCOMPLETE);
 puts("PASS: IPC v1 11 message types, framing, endian, validation and malformed packets");
 return 0;
}
