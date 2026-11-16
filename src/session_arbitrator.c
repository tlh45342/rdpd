#include "ipc_transport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
static volatile sig_atomic_t running=1;
static void stop(int sig){(void)sig;running=0;}
static int respond(int fd,uint16_t type,uint32_t session,uint32_t request,
                   uint32_t a,uint32_t b)
{
 uint8_t payload[8];
 ipc_message m;
 ipc_put_u32(payload,a);ipc_put_u32(payload+4,b);
 m.type=type;m.session_id=session;m.request_id=request;
 m.payload=payload;m.payload_length=8;
 return ipc_send_fd(fd,&m);
}
static void handle(int fd)
{
 uint8_t *buffer=(uint8_t *)malloc(IPC_MAX_FRAME);
 ipc_message m;
 uint32_t session=0;
 int ready=0,open=0;
 if(!buffer)return;
 while(ipc_recv_fd(fd,&m,buffer,IPC_MAX_FRAME)==0) {
  if(!ready) {
   if(m.type!=IPC_HELLO || ipc_get_u32(m.payload)>IPC_VERSION ||
      ipc_get_u32(m.payload+4)<IPC_VERSION)break;
   if(respond(fd,IPC_HELLO_ACK,0,m.request_id,IPC_VERSION,0)!=0)break;
   ready=1;
   continue;
  }
  if(m.type==IPC_SESSION_OPEN && !open && m.session_id!=0) {
   if(ipc_get_u32(m.payload)==0 || ipc_get_u32(m.payload+4)==0)break;
   session=m.session_id;open=1;
   if(respond(fd,IPC_STATUS,session,m.request_id,0,0)!=0)break;
  } else if(m.type==IPC_HEARTBEAT) {
   if(respond(fd,IPC_STATUS,session,m.request_id,0,0)!=0)break;
  } else if(m.type==IPC_SESSION_CLOSE && open && m.session_id==session) {
   (void)respond(fd,IPC_STATUS,session,m.request_id,0,0);
   break;
  } else if(open && m.session_id==session &&
            (m.type==IPC_POINTER || m.type==IPC_KEY ||
             m.type==IPC_APP_LAUNCH || m.type==IPC_WINDOW_ACTION)) {
   /* Contract validation only. Desktop dispatch is a later sprint. */
   if(respond(fd,IPC_STATUS,session,m.request_id,1,0)!=0)break;
  } else break;
 }
 free(buffer);
}
int main(int argc,char **argv)
{
 struct sockaddr_un address;
 int server,client;
 const char *path=argc==2?argv[1]:"/tmp/rdpd-arbitrator.sock";
 if(argc>2 || strlen(path)>=sizeof(address.sun_path))return 2;
 signal(SIGINT,stop);signal(SIGTERM,stop);signal(SIGPIPE,SIG_IGN);
 server=socket(AF_UNIX,SOCK_STREAM,0);
 if(server<0)return 1;
 memset(&address,0,sizeof(address));address.sun_family=AF_UNIX;
 strcpy(address.sun_path,path);
 /* Refuse to remove arbitrary pre-existing paths. */
 if(access(path,F_OK)==0) {
  fprintf(stderr,"socket path already exists: %s\n",path);
  close(server);return 1;
 }
 if(bind(server,(struct sockaddr *)&address,sizeof(address))!=0) {
  perror("bind");close(server);return 1;
 }
 chmod(path,0600);
 if(listen(server,8)!=0){perror("listen");close(server);unlink(path);return 1;}
 fprintf(stderr,"session_arbitrator: listening on %s\n",path);
 while(running) {
  client=accept(server,0,0);
  if(client<0){if(errno==EINTR)continue;break;}
  handle(client);
  close(client);
 }
 close(server);unlink(path);
 return 0;
}
#else
int main(void)
{
 fputs("session_arbitrator: Unix socket transport requires POSIX\n",stderr);
 return 2;
}
#endif
