#include "rdpd.h"
#include "protocol.h"
#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#define CLOSESOCKET closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define CLOSESOCKET close
#endif

int rdpd_serve(unsigned short port, volatile sig_atomic_t *running)
{
    socket_t listener;
    struct sockaddr_in addr;
    int result = 1;
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa)) { fprintf(stderr, "WSAStartup failed\n"); return 1; }
#endif
    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) { perror("socket"); goto cleanup; }
    {
        int yes = 1;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        fprintf(stderr, "bind failed (port %u)\n", (unsigned)port);
        goto close_listener;
    }
    if (listen(listener, 8) == SOCKET_ERROR) { fprintf(stderr, "listen failed\n"); goto close_listener; }
    printf("listening on 0.0.0.0:%u (Ctrl+C to stop)\n", (unsigned)port);
    fflush(stdout);
    result = 0;
    while (*running) {
        struct sockaddr_in peer;
#ifdef _WIN32
        int len = sizeof(peer);
        fd_set fds;
        struct timeval tv = { 0, 250000 };
        FD_ZERO(&fds); FD_SET(listener, &fds);
        if (select(0, &fds, NULL, NULL, &tv) <= 0) continue;
#else
        socklen_t len = sizeof(peer);
        fd_set fds;
        struct timeval tv = { 0, 250000 };
        FD_ZERO(&fds); FD_SET(listener, &fds);
        if (select(listener + 1, &fds, NULL, NULL, &tv) <= 0) continue;
#endif
        {
            socket_t client = accept(listener, (struct sockaddr *)&peer, &len);
            if (client == INVALID_SOCKET) continue;
            printf("connection from %s:%u\n", inet_ntoa(peer.sin_addr), (unsigned)ntohs(peer.sin_port));
            fflush(stdout);
            rdpd_handle_connection((uintptr_t)client);
            CLOSESOCKET(client);
        }
    }
close_listener:
    CLOSESOCKET(listener);
cleanup:
#ifdef _WIN32
    WSACleanup();
#endif
    return result;
}
