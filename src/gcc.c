#include "gcc.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* GCC ConferenceCreateResponse PER envelope and RDP server data blocks.
   Static conference identifier is the standard 'McDn'; all lengths computed. */
int rdpd_build_gcc_response(unsigned char *out, size_t cap, size_t *used)
{
    static const unsigned char prefix[] = {
        0x00,0x05,0x00,0x14,0x7c,0x00,0x01,0x2a,0x14,0x76,
        0x0a,0x01,0x01,0x00,0x01,0xc0,0x00,0x4d,0x63,0x44,0x6e
    };
    static const unsigned char blocks[] = {
        /* SC_CORE: type 0x0c01, length 12, RDP version 0x00080004 */
        0x01,0x0c,0x0c,0x00,0x04,0x00,0x08,0x00,0x00,0x00,0x00,0x00,
        /* SC_SECURITY: standard RDP encryption disabled, no certificate */
        0x02,0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        /* SC_NET: global MCS channel 1003, zero static virtual channels */
        0x03,0x0c,0x08,0x00,0xeb,0x03,0x00,0x00
    };
    size_t total = sizeof(prefix) + 2 + sizeof(blocks);
    if (!out || !used || cap < total || sizeof(blocks) > 0x7fff) return -1;
    memcpy(out, prefix, sizeof(prefix));
    /* PER length determinant for server data payload */
    out[sizeof(prefix)] = (unsigned char)(0x80 | (sizeof(blocks) >> 8));
    out[sizeof(prefix)+1] = (unsigned char)sizeof(blocks);
    memcpy(out + sizeof(prefix) + 2, blocks, sizeof(blocks));
    *used = total;
    return 0;
}

/* Diagnostic bounded scan of GCC client block headers. No untrusted offsets
   are used to construct a response; unknown/malformed blocks are ignored. */
void rdpd_log_gcc_client(const unsigned char *p, size_t n)
{
    size_t i;
    for (i = 0; i + 8 <= n; ++i) {
        if (p[i] == 0x01 && p[i+1] == 0xc0) {
            size_t length = (size_t)p[i+2] | ((size_t)p[i+3] << 8);
            if (length >= 12 && length <= n-i) {
                unsigned width = (unsigned)p[i+8] | ((unsigned)p[i+9] << 8);
                unsigned height = (unsigned)p[i+10] | ((unsigned)p[i+11] << 8);
                if (length >= 12 && width >= 200 && width <= 16384 &&
                    height >= 200 && height <= 16384)
                    printf("  GCC client core: desktop=%ux%u\n", width, height);
                else puts("  GCC client core detected");
                return;
            }
        }
    }
    puts("  GCC client core not identified (diagnostic scan)");
}
