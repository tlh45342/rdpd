#include "ipc.h"
#include <string.h>
static uint16_t get16(const uint8_t *p)
{ return (uint16_t)(((uint16_t)p[0]<<8)|p[1]); }
static void put16(uint8_t *p,uint16_t v)
{ p[0]=(uint8_t)(v>>8);p[1]=(uint8_t)v; }
uint32_t ipc_get_u32(const uint8_t *p)
{ return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3]; }
void ipc_put_u32(uint8_t *p,uint32_t v)
{ p[0]=(uint8_t)(v>>24);p[1]=(uint8_t)(v>>16);p[2]=(uint8_t)(v>>8);p[3]=(uint8_t)v; }
static int valid_type(uint16_t type)
{ return type>=IPC_HELLO && type<=IPC_HEARTBEAT; }
static int valid_payload(uint16_t type,const uint8_t *p,uint32_t n)
{
 uint32_t action;
 switch(type) {
 case IPC_HELLO: case IPC_HELLO_ACK: return n==8;
 case IPC_SESSION_OPEN: return n==8; /* width, height */
 case IPC_SESSION_CLOSE: return n==0;
 case IPC_POINTER: return n==12 && ipc_get_u32(p+8)<=2;
 case IPC_KEY: return n==8; /* scancode, flags */
 case IPC_APP_LAUNCH: {
  uint32_t i;
  if(n==0 || n>128)return 0;
  for(i=0;i<n;i++)
   if(!((p[i]>='a' && p[i]<='z') || (p[i]>='0' && p[i]<='9') ||
        p[i]=='-' || p[i]=='_'))return 0;
  return 1;
 }
 case IPC_WINDOW_ACTION:
  if(n!=8)return 0;
  action=ipc_get_u32(p+4);
  return action>=IPC_WINDOW_FOCUS && action<=IPC_WINDOW_CLOSE;
 case IPC_FRAME_DAMAGE: return n==16; /* x,y,width,height */
 case IPC_STATUS: return n==8; /* status code, detail */
 case IPC_HEARTBEAT: return n==0;
 default:return 0;
 }
}
int ipc_encode(uint8_t *out,size_t cap,const ipc_message *m,size_t *written)
{
 size_t total;
 if(written)*written=0;
 if(!out || !m || !written || !valid_type(m->type))return IPC_INVALID;
 if(m->payload_length>IPC_MAX_PAYLOAD)return IPC_TOO_LARGE;
 if(m->payload_length && !m->payload)return IPC_INVALID;
 if(!valid_payload(m->type,m->payload,m->payload_length))return IPC_INVALID;
 total=IPC_HEADER_SIZE+(size_t)m->payload_length;
 if(cap<total)return IPC_NO_SPACE;
 out[0]='R';out[1]='D';out[2]='I';out[3]='P';
 put16(out+4,IPC_VERSION);put16(out+6,m->type);
 ipc_put_u32(out+8,m->session_id);
 ipc_put_u32(out+12,m->request_id);
 ipc_put_u32(out+16,m->payload_length);
 ipc_put_u32(out+20,0);
 if(m->payload_length)memmove(out+IPC_HEADER_SIZE,m->payload,m->payload_length);
 *written=total;
 return IPC_OK;
}
int ipc_frame_length(const uint8_t *b,size_t n,size_t *total)
{
 uint32_t len;
 if(!b || !total)return IPC_INVALID;
 *total=0;
 if(n<IPC_HEADER_SIZE)return IPC_INCOMPLETE;
 if(memcmp(b,"RDIP",4)!=0 || ipc_get_u32(b+20)!=0)return IPC_INVALID;
 if(get16(b+4)!=IPC_VERSION)return IPC_UNSUPPORTED;
 if(!valid_type(get16(b+6)))return IPC_UNSUPPORTED;
 len=ipc_get_u32(b+16);
 if(len>IPC_MAX_PAYLOAD)return IPC_TOO_LARGE;
 *total=IPC_HEADER_SIZE+(size_t)len;
 return n<*total?IPC_INCOMPLETE:IPC_OK;
}
int ipc_decode(const uint8_t *b,size_t n,ipc_message *m,size_t *consumed)
{
 size_t total;
 int status;
 if(consumed)*consumed=0;
 if(!m || !consumed)return IPC_INVALID;
 status=ipc_frame_length(b,n,&total);
 if(status!=IPC_OK)return status;
 if(!valid_payload(get16(b+6),b+IPC_HEADER_SIZE,ipc_get_u32(b+16)))
  return IPC_INVALID;
 m->type=get16(b+6);
 m->session_id=ipc_get_u32(b+8);
 m->request_id=ipc_get_u32(b+12);
 m->payload_length=ipc_get_u32(b+16);
 m->payload=b+IPC_HEADER_SIZE;
 *consumed=total;
 return IPC_OK;
}
