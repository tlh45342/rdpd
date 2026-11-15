#include "mcs_response.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Bounded BER writer. A minimal T.125 response, not a complete RDP GCC response. */
struct writer { unsigned char *p; size_t n, cap; };
static int put(struct writer *w, unsigned char b)
{
    if (w->n == w->cap) return -1;
    w->p[w->n++] = b;
    return 0;
}
static int bytes(struct writer *w, const unsigned char *p, size_t n)
{
    if (n > w->cap - w->n) return -1;
    memcpy(w->p + w->n, p, n); w->n += n; return 0;
}
static int ber_length(struct writer *w, size_t n)
{
    if (n < 128) return put(w, (unsigned char)n);
    if (n < 256) return put(w, 0x81) || put(w, (unsigned char)n);
    return put(w, 0x82) || put(w, (unsigned char)(n >> 8)) ||
           put(w, (unsigned char)n);
}
static int tlv(struct writer *w, unsigned char tag,
               const unsigned char *p, size_t n)
{
    return put(w, tag) || ber_length(w, n) || bytes(w, p, n);
}
int rdpd_build_mcs_response(unsigned char *out, size_t cap, size_t *used)
{
    static const unsigned char domain[] = {
        0x02,0x01,0x22, 0x02,0x01,0x02, 0x02,0x01,0x00,
        0x02,0x01,0x01, 0x02,0x01,0x00, 0x02,0x01,0x01,
        0x02,0x02,0xff,0xff, 0x02,0x01,0x02
    };
    static const unsigned char result[] = {0x00};
    static const unsigned char connect_id[] = {0x00};
    unsigned char body[128], frame[256];
    struct writer b = {body, 0, sizeof(body)};
    struct writer f = {frame, 0, sizeof(frame)};
    struct writer o = {out, 0, cap};
    /* Connect-Response ::= [APPLICATION 102] SEQUENCE {
       result ENUMERATED, calledConnectId INTEGER,
       domainParameters DomainParameters, userData OCTET STRING }
       Empty userData is diagnostic only: GCC ConferenceCreateResponse
       and server security/channel data still need implementation. */
    if (tlv(&b, 0x0a, result, sizeof(result)) ||
        tlv(&b, 0x02, connect_id, sizeof(connect_id)) ||
        tlv(&b, 0x30, domain, sizeof(domain)) ||
        tlv(&b, 0x04, NULL, 0)) return -1;
    if (put(&f, 0x7f) || put(&f, 0x66) || ber_length(&f, b.n) ||
        bytes(&f, body, b.n)) return -1;
    if (f.n + 7 > 65535 || f.n + 7 > cap) return -1;
    if (put(&o, 3) || put(&o, 0) ||
        put(&o, (unsigned char)((f.n + 7) >> 8)) ||
        put(&o, (unsigned char)(f.n + 7)) ||
        put(&o, 2) || put(&o, 0xf0) || put(&o, 0x80) ||
        bytes(&o, frame, f.n)) return -1;
    *used = o.n;
    return 0;
}
