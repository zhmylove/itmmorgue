// vim: sw=4 ts=4 et :
#include "server.h"

void server() {
    int s, rc, one = 1;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t client_len;

    if (server_started != 0) {
        panic("Server is already running!");
    }

    server_started = 1;

    addr.sin_len         = sizeof(struct sockaddr_in);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        panic("Unable to create server socket!");
    }

    if ((rc = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const void *)&one,
                    sizeof(int))) < 0) {
        panic("Unable to set server socket SO_REUSEADDR!");
    }

    if ((rc = bind(s, (const struct sockaddr *)&addr, sizeof(addr))) < 0) {
        panic("Unable to bind server socket!");
    }

    if ((rc = listen(s, SERVER_BACKLOG)) < 0) {
        panic("Unable to set server socket backlog!");
    }

    // TODO revise multithreaded approach
    while ((rc = accept(s, (struct sockaddr *)&client, &client_len)) >= 0) {
        write(rc, ",japh\r\n", 7);
        close(rc);
    }

    panic("Server exited abnormaly");
}

void sigchld(int signum) {
    panic("Received SIGCHLD!");

    (void)signum;
}

void server_start() {
    struct sigaction sa_chld;
    sa_chld.sa_handler = &sigchld;
    sigaction(SIGCHLD, &sa_chld, NULL);

    if (server_started != 0) {
        panic("Server process is already running!");
    }

    if ((server_started = fork()) < 0) {
        panic("Unable to create server process!");
    }

    if (server_started == 0) {
        server();
        exit(0);
    }
}
