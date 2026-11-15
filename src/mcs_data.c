#include "mcs_data.h"
#include "client_info.h"
#include <stdio.h>

static unsigned be16(const unsigned char *p)
{
    return ((unsigned)p[0] << 8) | p[1];
}
static unsigned le16(const unsigned char *p)
{
    return (unsigned)p[0] | ((unsigned)p[1] << 8);
}
static void dump(const unsigned char *p, size_t n)
{
    size_t i, shown = n < 64 ? n : 64;
    printf("  RDP payload hex (%u/%u bytes):", (unsigned)shown, (unsigned)n);
    for (i=0; i<shown; ++i) {
        if (!(i%16)) printf("\n    ");
        printf("%02x ",p[i]);
    }
    putchar('\n');
}
/* MCS SendDataRequest PER:
   opcode, initiator (wire userId - 1001), channelId, priority/segmentation,
   PER length determinant, userData. We do not decrypt or execute payloads. */
int rdpd_inspect_send_data(const unsigned char *p, size_t n)
{
    size_t offset=13, payload_length;
    unsigned initiator, channel, priority, segmentation, flags;
    if (!p || n<14 || p[0]!=3 || p[1]!=0 || be16(p+2)!=n ||
        p[4]!=2 || p[5]!=0xf0 || p[6]!=0x80 || p[7]!=0x64)
        return -1;
    initiator=be16(p+8)+1001;
    channel=be16(p+10);
    priority=(p[12]>>6)&3;
    segmentation=(p[12]>>4)&3;
    if (p[13]&0x80) {
        if (n<15) return -1;
        payload_length=((size_t)(p[13]&0x7f)<<8)|p[14];
        offset=15;
        if (payload_length<128) return -1;
    } else {
        payload_length=p[13];
        offset=14;
    }
    if (payload_length!=n-offset || initiator!=1002 ||
        (channel!=1002 && channel!=1003)) return -1;
    printf("  MCS SendDataRequest: initiator=%u channel=%u priority=%u segmentation=%u payload=%u\n",
           initiator,channel,priority,segmentation,(unsigned)payload_length);
    if (payload_length>=4) {
        flags=le16(p+offset);
        printf("  RDP security header candidate: flags=0x%04x flagsHi=0x%04x\n",
               flags,le16(p+offset+2));
        if (flags & 0x0001) puts("  RDP security: SEC_EXCHANGE_PKT candidate");
        if (flags & 0x0040) puts("  RDP security: SEC_INFO_PKT candidate");
        if (flags & 0x0008) puts("  RDP security: SEC_ENCRYPT set (unsupported)");
    } else puts("  RDP payload too short for security header");
    /* Client Info may contain usernames and passwords. Never hex-dump it. */
    if (payload_length>=4 && (le16(p+offset)&0x0040)) {
        int result;
        puts("  Client Info payload redacted (may contain credentials)");
        if (le16(p+offset)&0x0008) {
            puts("  Client Info encrypted: cannot authenticate without decryption");
            return -1;
        }
        result=rdpd_parse_client_info(p+offset+4,payload_length-4);
        if (result<0) puts("  Client Info malformed or unsupported");
        if (result<=0) return -1;
        puts("  Authentication accepted locally; licensing/activation response not implemented");
    }
    else if (payload_length>=4 && (le16(p+offset)&0x0008))
        puts("  Encrypted payload redacted");
    else dump(p+offset,payload_length);
    return 0;
}
