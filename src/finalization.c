#include "finalization.h"
#include "desktop.h"
#include <stdio.h>
#include <string.h>

static unsigned u16(const unsigned char *p) { return (unsigned)p[0] | ((unsigned)p[1]<<8); }
static void w16(unsigned char *p,unsigned v) { p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8); }
static void w32(unsigned char *p,unsigned long v) { w16(p,(unsigned)v);w16(p+2,(unsigned)(v>>16)); }

/* Share Data PDU is wrapped in TPKT, X.224 and MCS SendDataRequest. */
int rdpd_inspect_finalization(const unsigned char *p,size_t n)
{
    size_t off=14,len;
    const unsigned char *s;
    unsigned type,subtype;
    if (!p || n<14 || p[7]!=0x64 || p[4]!=2 || p[5]!=0xf0 || p[6]!=0x80) return -1;
    len=p[13];
    if (len&0x80) {
        if (n<15) return -1;
        len=((len&127)<<8)|p[14];off=15;
    }
    if (off+len!=n || len<6) return -1;
    s=p+off;
    type=u16(s+2)&15;
    if (u16(s)!=len) return -1;
    if (type!=7) {
        printf("  post-activation Share Control type=%u\n",type);
        return 0;
    }
    if (len<18) return -1;
    subtype=s[14];
    switch (subtype) {
    case 31: puts("  Client Synchronize received");break;
    case 20:
        if (len<26) return -1;
        printf("  Client Control received: action=%u\n",u16(s+18));
        break;
    case 39: puts("  Client Font List received");return 1;
    case 28:
        if (len>=22) {
            unsigned count=u16(s+18),i;
            if (count<256 && len>=22+(size_t)count*12) {
                for (i=0;i<count;i++) {
                    const unsigned char *event=s+22+(size_t)i*12;
                    unsigned message=u16(event+4),flags=u16(event+6);
                    if (message==0x8001 && (flags&0x9000)==0x9000) {
                        rdpd_desktop_click(u16(event+8),u16(event+10));
                        puts("  Desktop mouse click received");
                        return 2;
                    }
                }
            }
        }
        break;
    case 36: puts("  Client Shutdown Request received");break;
    default: printf("  Client Share Data subtype=%u\n",subtype);break;
    }
    return 0;
}

/* Minimal uncompressed Share Data PDUs. */
int rdpd_build_finalization(unsigned index,unsigned char *out,size_t cap,size_t *used)
{
    unsigned char share[40]={0};
    unsigned type,body;
    size_t n,total;
    if (!out || !used || index>3) return -1;
    type=index==0?31:index==3?40:20;
    body=index==0?4:index==3?8:8;
    n=18+body;
    w16(share,(unsigned)n);
    w16(share+2,0x0017); /* Share Data PDU */
    w16(share+4,1002);
    w32(share+6,0x000103ea);
    share[10]=0;
    share[11]=1; /* STREAM_LOW */
    w16(share+12,(unsigned)n);
    share[14]=(unsigned char)type;
    share[15]=0;
    w16(share+16,0);
    if (index==0) {
        w16(share+18,1);w16(share+20,1002);
    } else if (index==1 || index==2) {
        w16(share+18,index==1?4:2); /* cooperate / granted */
    } else {
        w16(share+18,0);w16(share+20,0);w16(share+22,3);w16(share+24,0);
    }
    total=14+n;
    if (cap<total) return -1;
    out[0]=3;out[1]=0;out[2]=(unsigned char)(total>>8);out[3]=(unsigned char)total;
    out[4]=2;out[5]=0xf0;out[6]=0x80;
    out[7]=0x68;out[8]=0;out[9]=1;out[10]=3;out[11]=0xeb;out[12]=0x70;
    out[13]=(unsigned char)n;
    memcpy(out+14,share,n);
    *used=total;
    return 0;
}
