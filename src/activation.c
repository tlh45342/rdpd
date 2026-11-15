#include "activation.h"
#include <stdio.h>
#include <string.h>

static void put16(unsigned char *p, unsigned v) { p[0]=(unsigned char)v; p[1]=(unsigned char)(v>>8); }
static void put32(unsigned char *p, unsigned long v) { put16(p,(unsigned)v); put16(p+2,(unsigned)(v>>16)); }
static unsigned get16(const unsigned char *p) { return (unsigned)p[0] | ((unsigned)p[1]<<8); }
static unsigned long get32(const unsigned char *p) { return (unsigned long)get16(p) | ((unsigned long)get16(p+2)<<16); }

/* Minimal RDP 5.x capability advertisement: general and bitmap.
   No orders, compression, surface commands, or virtual channels. */
int rdpd_build_demand_active(unsigned char *out, size_t cap, size_t *used)
{
    unsigned char share[128]={0};
    unsigned char *p=share;
    size_t n, total;
    static const unsigned char source[]={'r','d','p','d'};
    if (!out || !used) return -1;
    p+=6; /* Share Control Header */
    put32(p,0x000103ea); p+=4; /* shareId */
    put16(p,sizeof(source)); p+=2;
    put16(p,4+24+28); p+=2; /* combinedCapabilities length */
    memcpy(p,source,sizeof(source)); p+=sizeof(source);
    put16(p,2); p+=2; /* capability count */
    p+=2; /* padding */
    put16(p,1); put16(p+2,24); /* General Capability Set */
    put16(p+4,1); put16(p+6,3); /* OS major Windows, minor NT */
    put16(p+8,0x0200); /* protocol version */
    p+=24;
    put16(p,2); put16(p+2,28); /* Bitmap Capability Set */
    put16(p+4,16); /* preferred bits per pixel */
    put16(p+6,1); put16(p+8,1); put16(p+10,1); /* receive depth flags */
    put16(p+12,800); put16(p+14,600);
    put16(p+18,1); /* desktop resize */
    put16(p+20,1); /* bitmap compression */
    p+=28;
    n=(size_t)(p-share);
    put16(share,(unsigned)n);
    put16(share+2,0x0011); /* PDU_TYPE_DEMANDACTIVE | version 1 */
    put16(share+4,1002); /* pduSource */
    total=7+6+1+n; /* TPKT+X224, MCS fixed, PER short length */
    if (n>=128 || total>cap) return -1;
    out[0]=3;out[1]=0;out[2]=(unsigned char)(total>>8);out[3]=(unsigned char)total;
    out[4]=2;out[5]=0xf0;out[6]=0x80;
    out[7]=0x68;out[8]=0;out[9]=1;out[10]=3;out[11]=0xeb;out[12]=0x70;
    out[13]=(unsigned char)n;
    memcpy(out+14,share,n);
    *used=total;
    return 0;
}

int rdpd_inspect_confirm_active(const unsigned char *p, size_t n)
{
    size_t off, len;
    const unsigned char *s;
    if (!p || n<14 || p[7]!=0x64) return -1;
    off=14;
    len=p[13];
    if (len&0x80) {
        if (n<15) return -1;
        len=((len&0x7f)<<8)|p[14];off=15;
    }
    if (len!=n-off || len<6) return -1;
    s=p+off;
    if (get16(s+2)!=0x0013 || get16(s)!=len) return 0;
    if (len<16) return -1;
    printf("  Client Confirm Active: shareId=0x%08lx, payload=%u bytes\n",
           get32(s+6),(unsigned)len);
    return 1;
}
