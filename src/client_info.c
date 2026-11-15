#include "client_info.h"
#include <stdio.h>
#include <string.h>

static unsigned le16(const unsigned char *p)
{
    return (unsigned)p[0] | ((unsigned)p[1]<<8);
}
static unsigned long le32(const unsigned char *p)
{
    return (unsigned long)p[0] | ((unsigned long)p[1]<<8) |
           ((unsigned long)p[2]<<16) | ((unsigned long)p[3]<<24);
}
/* TS_INFO_PACKET: codePage, flags, five lengths, then five
   length-delimited strings each followed by a UTF-16 terminator.
   Only Unicode credentials are supported in this development mode. */
int rdpd_parse_client_info(const unsigned char *p, size_t n)
{
    static const char *fields[]={"domain","username","password","shell","directory"};
    static const char *expected[]={"","root","toor","",""};
    size_t off=18, i, j, len[5];
    unsigned long flags;
    int valid=1;
    if (!p || n<18) return -1;
    flags=le32(p+4);
    if (!(flags & 0x10)) {
        puts("  Client Info: non-Unicode credentials unsupported");
        return -1;
    }
    for (i=0;i<5;i++) {
        len[i]=le16(p+8+i*2);
        if ((len[i]&1) || len[i]>1024) return -1;
    }
    for (i=0;i<5;i++) {
        size_t chars=len[i]/2, expected_len=strlen(expected[i]);
        if (off>n || n-off<len[i]+2) return -1;
        if (p[off+len[i]] || p[off+len[i]+1]) return -1;
        if (i==1 || i==2) {
            if (chars!=expected_len) valid=0;
            for (j=0;j<chars;j++) {
                unsigned c=le16(p+off+j*2);
                if (j>=expected_len || c!=(unsigned char)expected[i][j])
                    valid=0;
            }
        }
        off+=len[i]+2;
    }
    printf("  Client Info: Unicode fields valid, flags=0x%08lx, extended=%u bytes\n",
           flags,(unsigned)(n-off));
    (void)fields;
    puts(valid ? "  Development authentication: ACCEPTED (root)" :
                 "  Development authentication: REJECTED");
    return valid;
}
