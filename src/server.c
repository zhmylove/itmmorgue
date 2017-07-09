// vim: sw=4 ts=4 et :
#include "server.h"

int log_fd = -1;

void logger(char *str) {
    if (log_fd < 0) {
        return;
    }

    if (write(log_fd, str, strlen(str)) < 0) {
        panic("Unable to dump str to log!");
    }

    if (write(log_fd, "\n", 1) < 0) {
        panic("Unable to dump str to log!");
    }
}

void server() {
    int s, rc, one = 1;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t client_len;

    if (server_started != 0) {
        panic("Server is already running!");
    }

    server_started = 1;

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    char * log_file = conf("server_log_file").sval;
    if (! *log_file) {
        return;
    }

    if ((log_fd = open(log_file, O_WRONLY | O_APPEND | O_CREAT, 0666)) < 0) {
        panicf("Unable to open %s!", log_file);
    }

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
        msg_t msg;

        do {
            msg.type = MSG_ECHO_REQUEST;
            msg.size = 0;
            msg.payload[0] = 0;

            // TODO rewrite everything from the beginning
            if (write(rc, &msg, sizeof(msg_t)) < 0) {
                panic("Unable to send datagram to the client!");
                close(rc);
                break;
            }

            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(rc, &fds);
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000;

            if (select(rc + 1, &fds, NULL, NULL, &timeout) <= 0) {
                logger(".");
                continue;
            }

            if (read(rc, &msg, sizeof(msg_t)) < 0) {
                panic("Unable to get datagram from the client!");
                close(rc);
                break;
            }

            switch (msg.type) {
                case MSG_ECHO_REPLY:
                    logger("Got ECHO_REPLY from the client!");
                    break;
                default:
                    break;
            }
        } while (usleep(3000000) >= 0);
    }

    panic("Server exited abnormaly");
}

void sigchld(int signum) {
    if (end == 0) {
        panic("Received SIGCHLD!");
    }

    (void)signum;
}

void server_fork_start() {
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

    // client code
    struct sigaction sa_chld;
    sa_chld.sa_handler = &sigchld;
    sigaction(SIGCHLD, &sa_chld, NULL);
}
