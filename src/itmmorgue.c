// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "server.h"
#include "client.h"

int main(int argc, char *argv[]) {
    // TODO parse argv and run server / client

    server_started = 0;
    server_connected = 0;

    config_init("itmmorgue.conf");

    while (*++argv) {
        if (strcmp(*argv, "--server-only") == 0) {
            server_only = 1;
        } else if (strcmp(*argv, "-s") == 0) {
            server_only = 1;
        }
    }

    log_fd = -1;
    logger_init();

    if (server_only == 0) {
        client();
    } else {
        fprintf(stderr, "Starting server in headless mode...\n");
        server();
    }

    (void) argc;
    return EXIT_SUCCESS;
}
