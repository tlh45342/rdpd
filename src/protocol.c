#include "protocol.h"
#include "mcs.h"
#include "mcs_response.h"
#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET net_socket;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
typedef int net_socket;
#endif

#define MAX_TPKT 16384
static int recv_exact(net_socket fd, unsigned char *p, size_t n);

static int recv_tpkt(net_socket fd, unsigned char *packet, size_t *length)
{
    size_t n;
    if (recv_exact(fd, packet, 4) != 0) return -1;
    n = ((size_t)packet[2] << 8) | packet[3];
    if (packet[0] != 3 || packet[1] != 0 || n < 7 || n > MAX_TPKT) return -2;
    if (recv_exact(fd, packet + 4, n - 4) != 0) return -1;
    *length = n;
    return 0;
}

static int recv_exact(net_socket fd, unsigned char *p, size_t n)
{
    while (n) {
        int r = recv(fd, (char *)p, (int)n, 0);
        if (r <= 0) return -1;
        p += r;
        n -= (size_t)r;
    }
    return 0;
}

static int send_exact(net_socket fd, const unsigned char *p, size_t n)
{
    while (n) {
        int r = send(fd, (const char *)p, (int)n, 0);
        if (r <= 0) return -1;
        p += r;
        n -= (size_t)r;
    }
    return 0;
}

/* TPKT / X.224 Connection Request; accept optional RDP negotiation request. */
int rdpd_parse_x224(const unsigned char *p, size_t n, unsigned int *protocols)
{
    size_t li, end, pos;
    if (!p || !protocols || n < 11 || p[0] != 3 || p[1] != 0 ||
        ((size_t)p[2] << 8 | p[3]) != n) return -1;
    li = p[4];
    if (li < 6 || li + 5 > n || p[5] != 0xe0) return -1;
    end = 5 + li;
    /* A request may contain a Cookie: line before the negotiation structure. */
    pos = 11;
    *protocols = 0;
    while (pos < end) {
        if (end - pos >= 7 && p[pos] == 'C' && p[pos+1] == 'o' &&
            p[pos+2] == 'o' && p[pos+3] == 'k' && p[pos+4] == 'i' &&
            p[pos+5] == 'e' && p[pos+6] == ':') {
            while (pos + 1 < end && !(p[pos] == '\r' && p[pos+1] == '\n')) ++pos;
            if (pos + 1 >= end) return -1;
            pos += 2;
        } else if (end - pos == 8 && (p[pos] == 1 || p[pos] == 6) &&
                   p[pos+2] == 8 && p[pos+3] == 0) {
            *protocols = (unsigned int)p[pos+4] |
                         ((unsigned int)p[pos+5] << 8) |
                         ((unsigned int)p[pos+6] << 16) |
                         ((unsigned int)p[pos+7] << 24);
            pos += 8;
        } else return -1;
    }
    return end == n ? 0 : -1;
}

int rdpd_handle_connection(uintptr_t handle)
{
    net_socket fd = (net_socket)handle;
    unsigned char packet[MAX_TPKT];
    unsigned int protocols;
    size_t length;
#ifdef _WIN32
    DWORD timeout = 3000;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
#else
    struct timeval timeout = { 3, 0 };
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#endif
    if (recv_tpkt(fd, packet, &length) != 0 ||
        rdpd_parse_x224(packet, length, &protocols) != 0) {
        puts("  invalid X.224 Connection Request"); return -1;
    }
    printf("  X.224 Connection Request: %u bytes, requestedProtocols=0x%08x\n",
           (unsigned)length, protocols);
    /* No TLS/CredSSP yet: do not advertise a security mode we cannot run. */
    if (protocols) {
        static const unsigned char failure[] = {
            3,0,0,19, 14,0xd0,0,0,0,0,0, 3,0,8,0, 2,0,0,0
        };
        puts("  security negotiation unsupported; sending failure (SSL_NOT_ALLOWED_BY_SERVER)");
        fflush(stdout);
        return send_exact(fd, failure, sizeof(failure));
    }
    {
        static const unsigned char confirm[] = {
            3,0,0,11, 6,0xd0,0,0,0,0,0
        };
        puts("  sending X.224 Connection Confirm (legacy RDP security)");
        fflush(stdout);
        if (send_exact(fd, confirm, sizeof(confirm)) != 0) return -1;
        puts("  waiting for MCS Connect-Initial (3 second timeout)");
        fflush(stdout);
        if (recv_tpkt(fd, packet, &length) != 0) {
            puts("  MCS receive timeout, disconnect, or invalid TPKT");
            fflush(stdout);
            return -1;
        }
        {
            int result = rdpd_inspect_mcs(packet, length);
            if (result == 0) {
                unsigned char response[256];
                size_t response_length = 0;
                if (rdpd_build_mcs_response(response, sizeof(response),
                                            &response_length) != 0) return -1;
                puts("  sending experimental T.125 MCS Connect-Response (GCC not yet implemented)");
                fflush(stdout);
                if (send_exact(fd, response, response_length) != 0) return -1;
                if (recv_tpkt(fd, packet, &length) == 0) {
                    printf("  next client TPKT: %u bytes, X.224 type=0x%02x\n",
                           (unsigned)length, length > 5 ? packet[5] : 0);
                } else puts("  client disconnected or timed out after MCS response");
            }
            fflush(stdout);
            return result;
        }
    }
}
