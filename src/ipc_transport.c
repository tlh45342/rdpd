#include "ipc_transport.h"
#include <stdlib.h>
#ifdef _WIN32
#include <winsock2.h>
#define IPC_READ(fd,b,n) recv((SOCKET)(fd),(char *)(b),(int)(n),0)
#define IPC_WRITE(fd,b,n) send((SOCKET)(fd),(const char *)(b),(int)(n),0)
#else
#include <unistd.h>
#include <errno.h>
#define IPC_READ(fd,b,n) read(fd,b,n)
#define IPC_WRITE(fd,b,n) write(fd,b,n)
#endif
static int transfer(int fd,uint8_t *bytes,size_t count,int writing)
{
 size_t pos=0;
 while(pos<count) {
  int n=(int)(writing?IPC_WRITE(fd,bytes+pos,count-pos):
                          IPC_READ(fd,bytes+pos,count-pos));
  if(n<=0) {
#ifndef _WIN32
   if(n<0 && errno==EINTR)continue;
#endif
   return -1;
  }
  pos+=(size_t)n;
 }
 return 0;
}
int ipc_send_fd(int fd,const ipc_message *m)
{
 uint8_t *bytes=(uint8_t *)malloc(IPC_MAX_FRAME);
 size_t n=0;
 int result;
 if(!bytes)return -1;
 result=ipc_encode(bytes,IPC_MAX_FRAME,m,&n);
 if(result==IPC_OK)result=transfer(fd,bytes,n,1);
 free(bytes);
 return result==0?0:-1;
}
int ipc_recv_fd(int fd,ipc_message *m,uint8_t *bytes,size_t cap)
{
 size_t frame=0,used=0;
 int rc;
 if(!m || !bytes || cap<IPC_HEADER_SIZE)return -2;
 if(transfer(fd,bytes,IPC_HEADER_SIZE,0)!=0)return -1;
 rc=ipc_frame_length(bytes,IPC_HEADER_SIZE,&frame);
 if(rc!=IPC_OK && rc!=IPC_INCOMPLETE)return -2;
 if(frame>cap)return -2;
 if(transfer(fd,bytes+IPC_HEADER_SIZE,frame-IPC_HEADER_SIZE,0)!=0)return -1;
 rc=ipc_decode(bytes,frame,m,&used);
 return rc==IPC_OK && used==frame?0:-2;
}
