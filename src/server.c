// vim: sw=4 ts=4 et :
#include "server.h"

connection_t *first_connection;
connection_t *last_connection;

// TODO check / re-implement connected players checks
// TODO get rid of this shit
char start = 0;

void player_connected_off(size_t id) {
    if (start) {
        players[id]->player_context->connected = 0;
        players[id]->color ^= L_BLACK;
        if (players_total > 0) players_total--;
    } else {
        // TODO remove player[id]
    }

    if (players_total == 0 && start) {
        // TODO make smth else (?)
        logger("[S] No players left in. Server terminated successfully!");
        exit(EXIT_SUCCESS);
    }

    char join_msg[PLAYER_NAME_MAXLEN * 4];
    sprintf(join_msg,
            "Player %s has fallen out of the world!\n",
            players[id]->player_context->nickname);
    for (size_t i = 0; i < players_len; i++) {
        if (! players[i]->player_context->connected) continue;

        send_sysmsg(players[i]->player_context->connection,
                SM_PLAYER_LEFT, join_msg);
        s_send_players_full(players[i]);
    }
}

void server() {
    int s, rc, cs, one = 1;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    if (server_started != 0) {
        panic("Server is already running!");
    }

    // TODO do this asynchronously
    s_bt_init();
    s_levels_init();
    // Start event loop thread
    event_init();

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
    while (usleep(1000) == 0) {
        /* Waiting for new players */
        if ((cs = accept(s, (struct sockaddr *)&client, &client_len)) < 0) {
            break;
        }
        if (start) { /* Handle connections after !start */
            if (players_len == players_total) { /* Nobody left */
                // TODO Send graceful disconnect to client
                write(cs, ".", 1);
                usleep(100000);
                close(cs);
                continue;
            } else {
                start = 3;
            }
        }
        connection_t *connection;
        if (NULL == (connection =
                    (connection_t*)malloc(sizeof(connection_t)))) {
            panic("Cannot allocate memory for client connection!");
        }
        connection->curr_client = client;
        connection->curr_client_len = client_len;
        connection->socket = cs;
        pthread_mutex_init(&connection->socket_mutex, NULL);
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

    /*
     * TODO move this section under obtaining color + nickname (really?)
     * s_send_players() requires this id.
     * The best solution is to fill this values here and change them
     * after reception of the actual ones.
     */
    size_t id = player_init(L_YELLOW, "bsi", connection);
    players[id]->player_context->connected = 1;

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

        /* Handle start state (see server.h) */
        if (start == 1 || (start > 0 &&
                    players[id]->player_context->start == 1)) {
            // TODO make some of this periodically (at the end of every tick)
            s_level_send(0, players[id]);
            s_area_send(0, players[id]);
            s_send_entities_full(players[id]);

            players[id]->player_context->start = 0;
            start = 2;
        }

        // send messages to the client if any
        while (mqueue_get(s2c_queue, &mbuf) > 0) {
            if (1 == send_mbuf(cs, &mbuf)) {
                panic("Error sending message in server thread!");
            }
        }

        do {
            timeout.tv_sec  = 0;
            timeout.tv_usec = 50000;
            rc = select(cs + 1, &fds, NULL, NULL, &timeout);
        } while (rc < 0 && errno == EINTR);

        if (rc < 0) {
            client_connected = 0;
            /* TODO make something graceful, wait for they */
            panic("client disconnected!");
            break;
        } else if (rc == 0) {
            continue;
        }

        logger("[S] waiting mbuf");

        if ((rc = synchronized_readall(&connection->socket_mutex, cs,
                        &mbuf.msg, sizeof(mbuf.msg))) == 0) {
            logger("[S] Client closed connection!");
            player_connected_off(id);
            close_connection(connection);
            pthread_exit(NULL);
        } else if (rc < 0) {
            loggerf("[S] Error reading from socket [%d][%s]!",
                    rc, strerror(errno));
            player_connected_off(id);
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
            case MSG_REPORT_NICKNAME:
                logger("[S] [REPORT_NICKNAME]");
                break;
            case MSG_MOVE_PLAYER:
                logger("[S] [MOVE_PLAYER]");
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

            if (synchronized_readall(&connection->socket_mutex, cs, payload,
                        mbuf.msg.size) != (ssize_t)mbuf.msg.size) {
                logger("[S] Error reading payload");
            }

            loggerf("[S] Received buf: [%s]", payload);
        }

        // Even not started messages routine
        switch (mbuf.msg.type) {
            // TODO MSG_REPORT_COLOR
            case MSG_REPORT_NICKNAME:
                if (mbuf.msg.size >= PLAYER_NAME_MAXLEN + 1) {
                    logger("[S] Long nickname received");
                    s2c_mbuf.msg.type = MSG_ERROR_NICKNAME;
                    s2c_mbuf.msg.size = strlen("Nickname is too long") + 1;
                    if (NULL == (s2c_mbuf.payload =
                                (char*)malloc(s2c_mbuf.msg.size))) {
                        panic("[S] Cannot allocate ERRROR_NICKNAME payload!");
                    }
                    strcpy(s2c_mbuf.payload, "Nickname is too long");
                    mqueue_put(s2c_queue, s2c_mbuf);

                    player_connected_off(id);
                    close_connection(connection);
                    pthread_exit(NULL);
                }
                /* Get the color */
                unsigned char color = ((char *)payload++)[0];
                players[id]->color = color - '0';
                /* Handle reconnects */
                for (size_t i = 0; start == 3 && i < players_len; i++) {
                    if (players[i]->player_context->connected) continue;
                    if (strcmp(players[i]->player_context->nickname,
                                payload) != 0) continue;

                    // Resurrect necessary fields of old player
                    players[i]->player_context->connection =
                        players[id]->player_context->connection;
                    players[i]->player_context->connected = 1;
                    players[i]->color ^= L_BLACK;

                    // TODO eliminate races (safely remove players[id])
                    memset(players[id]->player_context->nickname, 0,
                            PLAYER_NAME_MAXLEN);
                    players_len--;
                    players_total = players_len;

                    // Finally replace the id's
                    id = i;
                    players[id]->player_context->start = 1;
                }
                strncpy(players[id]->player_context->nickname, payload,
                        mbuf.msg.size);

                char join_msg[PLAYER_NAME_MAXLEN * 4];
                sprintf(join_msg,
                        "Player %s has found his place in the world!\n",
                        players[id]->player_context->nickname);
                for (connection_t *curr = first_connection; curr;
                        curr = curr->next) {
                    send_sysmsg(curr, SM_PLAYER_JOINED, join_msg);
                }

                for (size_t i = 0; i < players_len; i++) {
                    s_send_players_full(players[i]);
                }
                break;
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
                if (!start && (
                            strstr(payload, "!start\n") != NULL ||
                            strstr(payload, "!s\n") != NULL
                            )) {
                    players[id]->player_context->ready = 1;

                    for (size_t i = 0; i < players_len; i++) {
                        s_send_players_full(players[i]);
                    }
                    break;
                }
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

                free(payload);

                break;
            default: // Will be parsed later, if (start)
                break;
        }

        if (! start) {
            size_t wait = players_len;

            for (size_t i = 0; i < players_len; i++) {
                if (players[i]->player_context->ready) {
                    wait--;
                }
            }

            if (! wait) {
                start = 1;
                for (size_t i = 0; i < players_len; i++) {
                    players[i]->player_context->start = 1;
                }
                players_total = players_len;
            }

            continue;
        }

        if (players_len != players_total) { /* Skip any in-game actions */
            continue;
        }

        // Normal messages routine
        // TODO do smth with message

        // If MSG_MOVE_PLAYER
        player_move_t *move;

        switch (mbuf.msg.type) {
            case MSG_MOVE_PLAYER:
                switch ((enum keyboard) *payload) {
                    case K_MOVE_LEFT:
                    case K_MOVE_RIGHT:
                    case K_MOVE_UP:
                    case K_MOVE_DOWN:
                    case K_MOVE_LEFT_UP:
                    case K_MOVE_RIGHT_UP:
                    case K_MOVE_LEFT_DOWN:
                    case K_MOVE_RIGHT_DOWN:
                        // Will be freed during event handle
                        if ((move = (player_move_t *)malloc(
                                        sizeof(player_move_t))) == NULL) {
                            panic("[S] Error allocating player_move_t!");
                        }
                        move->player_id = id;
                        move->direction = (enum keyboard) *payload;
                        event_player_add(id, EV_MOVE, move);
                        break;
                    default:
                        panic("[S] invalid move received!");
                }
                break;
            case MSG_GET_CHAT: /* Already handled */
                break;
            case MSG_NEW_CHAT:
                break;
            case MSG_REPORT_NICKNAME:
                break;
            default:
                warnf("Unknown type: %d", mbuf.msg.type);
                logger("[S] [UNKNOWN]");
                continue;
        }
    } while (client_connected == 1);

    player_connected_off(id);
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

        // Never reaches due to server() has infinite loop
        panic("[S] Server has done infinite loop for the first time!");
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
