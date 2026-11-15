#include "mcs.h"
#include <stdio.h>

static void dump_bytes(const unsigned char *p, size_t n)
{
    size_t i, shown = n < 64 ? n : 64;
    printf("  payload hex (%u/%u bytes):", (unsigned)shown, (unsigned)n);
    for (i = 0; i < shown; ++i) {
        if (i % 16 == 0) printf("\n    ");
        printf("%02x ", p[i]);
    }
    putchar('\n');
}

/* TPKT header and X.224 Data TPDU precede T.125 BER MCS Connect-Initial. */
int rdpd_inspect_mcs(const unsigned char *p, size_t n)
{
    size_t ber_len, ber_header, payload;
    if (!p || n < 7 || p[0] != 3 || p[1] != 0 ||
        (((size_t)p[2] << 8) | p[3]) != n) {
        puts("  invalid next TPKT"); return -1;
    }
    printf("  TPKT received: %u bytes\n", (unsigned)n);
    if (p[4] != 2 || p[5] != 0xf0 || p[6] != 0x80) {
        puts("  not an X.224 Data TPDU");
        dump_bytes(p + 4, n - 4);
        return -1;
    }
    puts("  X.224 Data TPDU received");
    if (n < 10 || p[7] != 0x7f || p[8] != 0x65) {
        puts("  MCS Connect-Initial tag absent");
        dump_bytes(p + 7, n - 7);
        return -1;
    }
    ber_header = 10;
    if ((p[9] & 0x80) == 0) {
        ber_len = p[9] ;
    } else {
        size_t i, count = p[9] & 0x7f;
        if (!count || count > 2 || 10 + count > n) {
            puts("  invalid MCS BER length"); return -1;
        }
        ber_len = 0;
        for (i = 0; i < count; ++i)
            ber_len = (ber_len << 8) | p[10+i];
        ber_header += count;
        if (ber_len < 128) {
            puts("  noncanonical MCS BER length"); return -1;
        }
    }
    payload = n - ber_header;
    if (ber_len != payload) {
        printf("  MCS length mismatch: declared=%u actual=%u\n",
               (unsigned)ber_len, (unsigned)payload);
        return -1;
    }
    printf("  MCS Connect-Initial detected: %u BER payload bytes\n",
           (unsigned)ber_len);
    puts("  GCC user data is contained within MCS payload (not yet decoded)");
    dump_bytes(p + ber_header, payload);
    return 0;
}
