#include "mcs_domain.h"
#include "mcs_data.h"
#include "activation.h"
#include "finalization.h"
#include <stdio.h>
#include <string.h>

/* T.125 DomainMCSPDU uses PER. User IDs are transmitted with 1001
   subtracted (the PER lower bound). This server assigns user ID 1002. */
#define USER_ID 1002
#define USER_WIRE (USER_ID - 1001)
#define GLOBAL_CHANNEL 1003

static unsigned get16(const unsigned char *p)
{
    return ((unsigned)p[0] << 8) | p[1];
}
static int frame(const unsigned char *p, size_t n,
                 unsigned char *out, size_t cap, size_t *used)
{
    if (n + 7 > cap || n + 7 > 65535) return -1;
    out[0]=3; out[1]=0; out[2]=(unsigned char)((n+7)>>8);
    out[3]=(unsigned char)(n+7); out[4]=2; out[5]=0xf0; out[6]=0x80;
    memcpy(out+7,p,n); *used=n+7;
    return 1;
}
int rdpd_mcs_domain_step(const unsigned char *p, size_t n,
                         unsigned char *out, size_t cap,
                         size_t *used, int *state)
{
    unsigned char reply[8];
    unsigned user, channel;
    if (!p || !out || !used || !state || n < 8 ||
        p[0]!=3 || p[1]!=0 || get16(p+2)!=n ||
        p[4]!=2 || p[5]!=0xf0 || p[6]!=0x80) return -1;
    *used=0;
    if (*state == 0) {
        /* ErectDomainRequest: subHeight and subInterval PER integers.
           Length and opcode checked; no response is required. */
        if (p[7]!=0x04 || n!=12) return -1;
        printf("  MCS ErectDomainRequest: %u bytes\n",(unsigned)n);
        *state=1;
        return 0;
    }
    if (*state == 1) {
        if (p[7]!=0x28 || n!=8) return -1;
        puts("  MCS AttachUserRequest");
        reply[0]=0x2e; reply[1]=0; reply[2]=0;
        reply[3]=(unsigned char)USER_WIRE;
        *state=2;
        puts("  MCS AttachUserConfirm: result=success userId=1002");
        return frame(reply,4,out,cap,used);
    }
    if (*state == 2 && p[7] == 0x64) {
        int result=rdpd_inspect_send_data(p,n);
        if (result==2) {
            /* MCS SendDataIndication + SEC_LICENSE_PKT +
               LICENSE_ERROR_MESSAGE / STATUS_VALID_CLIENT. */
            static const unsigned char license[] = {
                0x68,0x00,0x01,0x03,0xeb,0x70,0x14,
                0x80,0x00,0x00,0x00,
                0xff,0x03,0x10,0x00,
                0x07,0x00,0x00,0x00,
                0x02,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00
            };
            *state=3;
            puts("  sending RDP licensing STATUS_VALID_CLIENT");
            return frame(license,sizeof(license),out,cap,used);
        }
        return result;
    }
    if (*state==4 && p[7]==0x64) {
        int confirm=rdpd_inspect_confirm_active(p,n);
        if (confirm==1) { *state=5; return 0; }
        if (confirm<0) return -1;
        puts("  awaiting Client Confirm Active");
        return 0;
    }
    if (*state>=5 && p[7]==0x64) {
        int kind=rdpd_inspect_finalization(p,n);
        if (kind<0) return -1;
        if (kind==1 && *state==5) { *state=6; puts("  Client Font List: finalizing RDP session"); }
        return 0;
    }
    if (*state==3 && p[7]==0x64) {
        return rdpd_inspect_send_data(p,n)==-1 ? -1 : 0;
    }
    if (*state == 2) {
        if (p[7]!=0x38 || n!=12) return -1;
        user=get16(p+8)+1001;
        channel=get16(p+10);
        printf("  MCS ChannelJoinRequest: userId=%u channelId=%u\n",
               user,channel);
        if (user!=USER_ID || (channel!=USER_ID &&
            channel!=GLOBAL_CHANNEL)) {
            puts("  MCS ChannelJoinRequest rejected: unsupported channel");
            return -1;
        }
        reply[0]=0x3e; reply[1]=0;
        reply[2]=p[8]; reply[3]=p[9];
        reply[4]=p[10]; reply[5]=p[11];
        reply[6]=p[10]; reply[7]=p[11];
        printf("  MCS ChannelJoinConfirm: channelId=%u success\n",channel);
        return frame(reply,8,out,cap,used);
    }
    return -1;
}
