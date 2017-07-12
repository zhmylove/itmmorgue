// vim: sw=4 ts=4 et :
#include "server.h"

void server() {
    int s, rc, cs, one = 1;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    if (server_started != 0) {
        panic("Server is already running!");
    }

    server_started = 1;

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

    // TODO implement workers
    while ((cs = accept(s, (struct sockaddr *)&client, &client_len)) >= 0) {
        mqueue_t s2c_queue;
        mqueue_init(&s2c_queue);

        struct sockaddr_in curr_client = client;
        socklen_t curr_client_len = client_len;
        client_len = sizeof(client); // for Solaris

        (void) curr_client; (void) curr_client_len;

        struct timeval timeout;
        timeout.tv_sec  = 0;
        timeout.tv_usec = 50000;

        int client_connected = 1;

        do {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(cs, &fds);
            mbuf_t mbuf;

            // send messages to the client if any
            while (mqueue_get(&s2c_queue, &mbuf) > 0) {
                if ((rc = write(cs, &mbuf.msg, sizeof(msg_t))) < 0) {
                    panic("Error sending message in server thread!");
                }

                if (mbuf.msg.size == 0) {
                    continue;
                }

                if ((rc = write(cs, mbuf.payload, mbuf.msg.size)) < 0) {
                    panic("Error sending message in worker!");
                }

                free(mbuf.payload);
            }

            do {
                rc = select(cs + 1, &fds, NULL, NULL, &timeout);
            } while (rc < 0 && errno == EINTR);

            if (rc < 0) {
                client_connected = 0;
                panic("client disconnected!");
                break;
            } else if (rc == 0) {
                continue;
            }

            if ((rc = read(cs, &mbuf.msg, sizeof(mbuf.msg))) == 0) {
                logger("Client closed connection!");
                continue;
            } else if (rc < 0) {
                logger("Error reading from socket!");
                client_connected = 0;
                break;
            }

            // rc > 0
            switch (mbuf.msg.type) {
                case MSG_NEW_CHAT:
                    logger("[S] [NEW_CHAT]");
                    break;
                default:
                    warnf("Unknown type: %d", mbuf.msg.type);
                    logger("[S] [UNKNOWN]");
                    continue;
            }

            char *payload;
            mbuf_t s2c_mbuf;
            size_t size;

            if (mbuf.msg.size > 0) {
                payload = malloc(mbuf.msg.size);

                if (payload == NULL) {
                    panic("Unable to allocate buffer for payload!");
                }

                if (readall(cs, payload, mbuf.msg.size) !=
                        (ssize_t)mbuf.msg.size) {
                    logger("Error reading payload");
                }

                loggerf("[S] Received buf: [%s]", payload);
            }

            // TODO do smth with message
            switch (mbuf.msg.type) {
                case MSG_NEW_CHAT:
                    size = strlen(payload) + 1;

                    s2c_mbuf.payload = malloc(size);
                    s2c_mbuf.msg.type = MSG_PUT_CHAT;
                    s2c_mbuf.msg.size = size;
                    s2c_mbuf.msg.version = 0x1;
                    memcpy(s2c_mbuf.payload, payload, size);

                    free(payload);

                    mqueue_put(&s2c_queue, s2c_mbuf);

                    break;
                default:
                    warnf("Unknown type: %d", mbuf.msg.type);
                    logger("[S] [UNKNOWN]");
                    continue;
            }
        } while (client_connected == 1);

        close(cs);
    }

    panic("Server exited abnormally");
}

/*
 * Client side SGICHLD handler only for server subprocess
 */
void sigchld(int signum) {
    if (connecting == 0 && end == 0) {
        panic("Received SIGCHLD!");
    }

    server_started = 0;

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
