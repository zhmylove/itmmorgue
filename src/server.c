// vim: sw=4 ts=4 et :
#include "server.h"

connection_t *first_connection;
connection_t *last_connection;

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

    /*
     * Here we define some stuff for common client-size submodules like chat.
     */

    if ((schat = malloc(2)) == NULL) {
        panic("Unable to allocate server chat buffer!");
    }
    schat[0] = '\0';

    // TODO implement workers
    while ((cs = accept(s, (struct sockaddr *)&client, &client_len)) >= 0) {
        connection_t *connection;
        if (NULL == (connection =
                (connection_t*)malloc(sizeof(connection_t)))) {
            panic("Cannot allocate memory for client connection!");
        }
        connection->curr_client = client;
        connection->curr_client_len = client_len;
        connection->socket = cs;
        connection->sysmsg_mask = ~0;
        if (NULL == (connection->mqueueptr =
                (mqueue_t*)malloc(sizeof(mqueue_t)))) {
            panic("Cannot allocate memory for client message queue!");
        }
        connection->next = NULL;

        if (NULL == first_connection) {
            first_connection = last_connection = connection;
            connection->prev = NULL;
        } else {
            connection->prev = last_connection;
            last_connection->next = connection;
            last_connection = connection;
        }

        client_len = sizeof(client); // for Solaris

        if (pthread_create(&connection->thread, NULL,
                    (void*(*)(void*))&process_client,
                    connection) != 0) {
            panic("Error creating processor thread!");
        }
    }

    panic("Server exited abnormally");
}

void* process_client(connection_t *connection) {
    int cs = connection->socket;
    int rc;

    if (pthread_detach(pthread_self()) != 0) {
        panic("Error detaching processor pthread!");
    }

    mqueue_t *s2c_queue = connection->mqueueptr;
    mqueue_init(s2c_queue);

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
        while (mqueue_get(s2c_queue, &mbuf) > 0) {
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

        if ((rc = readall(cs, &mbuf.msg, sizeof(mbuf.msg))) == 0) {
            logger("[S] Client closed connection!");
            close_connection(connection);
            pthread_exit(NULL);
        } else if (rc < 0) {
            loggerf("[S] Error reading from socket [%d][%s]!",
                    rc, strerror(errno));
            close_connection(connection);
            pthread_exit(NULL);
        }

        // rc > 0
        switch (mbuf.msg.type) {
            case MSG_NEW_CHAT:
                logger("[S] [NEW_CHAT]");
                break;
            case MSG_GET_CHAT:
                logger("[S] [GET_CHAT]");
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
                logger("[S] Error reading payload");
            }

            loggerf("[S] Received buf: [%s]", payload);
        }

        // TODO do smth with message
        switch (mbuf.msg.type) {
            case MSG_GET_CHAT:
                size = strlen(schat) + 1;

                if ((s2c_mbuf.payload = malloc(size)) == NULL) {
                    panic("Error allocating payload buffer!");
                }
                s2c_mbuf.msg.type = MSG_PUT_CHAT;
                s2c_mbuf.msg.size = size;
                memcpy(s2c_mbuf.payload, schat, size);

                loggerf("[S] Sending PUT: [%s] size=%zu", schat, size);
                mqueue_put(s2c_queue, s2c_mbuf);

                break;
            case MSG_NEW_CHAT:
                s_chat_add(&schat, payload);
                size = strlen(payload) + 1;

                s2c_mbuf.msg.type = MSG_PUT_CHAT;
                s2c_mbuf.msg.size = size;

                for (connection_t *curr = first_connection; curr;
                        curr = curr->next) {
                    // will be freed during mqueue_get()
                    if ((s2c_mbuf.payload = malloc(size)) == NULL) {
                        panic("Error allocating payload buffer!");
                    }
                    memcpy(s2c_mbuf.payload, payload, size);

                    mqueue_put(curr->mqueueptr, s2c_mbuf);

                    send_sysmsg(curr, SM_CHAT_NEW_MESSAGE,
                            "New message in your chat!\n");
                }

                //send_sysmsg(connection, SM_CHAT_NEW_MESSAGE,
                //        "New message in your chat!\n");
                free(payload);

                break;
            default:
                warnf("Unknown type: %d", mbuf.msg.type);
                logger("[S] [UNKNOWN]");
                continue;
        }
    } while (client_connected == 1);

    close_connection(connection);
    // TODO is it reachable point?
    return NULL;
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

        sigset_t sigset;
        sigfillset(&sigset);
        sigdelset(&sigset, SIGTERM);

        if (sigprocmask(SIG_SETMASK, &sigset, NULL) < 0) {
            logger("[S] Error setting signal mask!");
            panic("Error setting signal mask!");
        }

        server();

        exit(0);
    }

    // client code
    struct sigaction sa_chld;
    sa_chld.sa_handler = &sigchld;
    sigaction(SIGCHLD, &sa_chld, NULL);
}

/*
 * Creates MSG_PUT_SYSMSG message from *msg*, puts it into queue.
 * Drops messages, which *type* is not masked my sysmsg_mask of client.
 * 
 * queue : queue to put created message
 * type  : type of message
 * msg   : message payload
 */
void send_sysmsg(connection_t *connection, enum msg_sysmsg_type type,
        const char *msg) {
    if (NULL == connection) return;
    if ((type & connection->sysmsg_mask) == 0) {
        loggerf("[S] Dropping SYSMSG: [%s], type=%d", msg, type);
    }

    mbuf_t s2c_mbuf;
    size_t msg_len = strlen(msg) + 1;
    if ((s2c_mbuf.payload = malloc(msg_len)) == NULL) {
        panic("Error allocating payload buffer!");
    }
    s2c_mbuf.msg.type = MSG_PUT_SYSMSG;
    s2c_mbuf.msg.size = msg_len;
    memcpy(s2c_mbuf.payload, msg, msg_len);

    loggerf("[S] Sending SYSMSG: [%s] size=%zu", msg, msg_len);
    mqueue_put(connection->mqueueptr, s2c_mbuf);
}

/*
 * Deletes connection from list, closes socket, destroys mqueue, etc.
 *
 * connection : connection to close.
 */
void close_connection(connection_t *connection) {
    if (NULL == connection) return;

    // TODO: lock it!
    // TODO: lock it!
    // TODO: lock it!
    close(connection->socket);
    mqueue_destroy(connection->mqueueptr);
    free(connection->mqueueptr);
    if (NULL == connection->prev) {
        first_connection = connection->next;
    } else {
        connection->prev->next = connection->next;
    }
    if (NULL == connection->next) {
        last_connection = connection->prev;
    } else {
        connection->next->prev = connection->prev;
    }

    free(connection);
}
