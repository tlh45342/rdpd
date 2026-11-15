#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "rdpd.h"

static volatile sig_atomic_t running = 1;
static void stop_server(int signal_number) { (void)signal_number; running = 0; }

int main(int argc, char **argv)
{
    unsigned long port = RDPD_DEFAULT_PORT;
    char *end = NULL;
    if (argc > 2) { fprintf(stderr, "usage: rdpd [port]\n"); return 2; }
    if (argc == 2) {
        port = strtoul(argv[1], &end, 10);
        if (!argv[1][0] || *end || port == 0 || port > 65535) {
            fprintf(stderr, "invalid port\n"); return 2;
        }
    }
    signal(SIGINT, stop_server);
    signal(SIGTERM, stop_server);
    printf("rdpd 0.0.1 build 0013\n");
    return rdpd_serve((unsigned short)port, &running);
}
