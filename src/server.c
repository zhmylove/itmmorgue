// vim: sw=4 ts=4 et :
#include "server.h"

int log_fd = -1;

void logger(char *str) {
    if (log_fd < 0) {
        return;
    }

    char buf[8192];
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) {
        panic("Unable to get system time!");
    }

    int len = snprintf(buf, sizeof(buf), "%lu: %s\n", tv.tv_sec, str);
    if (write(log_fd, buf, len) < 0) {
        panic("Unable to write log output!");
    }
}

void server() {
    int s, rc, cs, one = 1;
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

    char *log_file = conf("server_log_file").sval;
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

    // TODO implement workers
    while ((cs = accept(s, (struct sockaddr *)&client, &client_len)) >= 0) {
        mbuf_t mbuf;

        while ((rc = read(cs, &mbuf, sizeof(mbuf))) > 0) {
            switch (mbuf.msg.type) {
                case MSG_NEW_CHAT:
                    warn("Got MSG_NEW_CHAT!");
                    break;
                default:
                    warnf("Unknown type: %d", mbuf.msg.type);
                    continue;
            }

            if (mbuf.msg.size > 0) {
                char *buf = malloc(mbuf.msg.size);
                if (buf == NULL) {
                    panic("Unable to allocate buffer for payload!");
                }

                if (read(cs, buf, mbuf.msg.size) != (ssize_t)mbuf.msg.size) {
                    warn("Error reading payload");
                }

                warnf("Received: [%s} \n", buf);
                logger(buf);
            }
        }

        if (rc == 0) {
            warn("Client closed connection!");
        } else {
            warn("Error reading from socket");
        }

        close(cs);
    }

    panic("Server exited abnormally");
}

/*
 * Client side SGICHLD handler only for server subprocess
 */
void sigchld(int signum) {
    if (end == 0) {
        panic("Received SIGCHLD!");
    }

    (void)signum;
}

/*
 * Client side function to create server subprocess
 */
void server_fork_start() {
    if (server_started != 0) {
        warn("Server process is already running!");
        return;
    }

    if ((server_started = fork()) < 0) {
        panic("Unable to create server process!");
    }

    if (server_started == 0) {
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        server();
        exit(0);
    }

    // client code
    struct sigaction sa_chld;
    sa_chld.sa_handler = &sigchld;
    sigaction(SIGCHLD, &sa_chld, NULL);
}
