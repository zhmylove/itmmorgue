// vim: sw=4 ts=4 et :
#include "itmmorgue.h"

void at_exit(void) {
    if (! isendwin()) {
        endwin();
    }

    if (server_started) {
        kill(server_started, SIGTERM);
    }
}

int connect_to_server(char *address) {
    if (sock >= 0) {
        close(sock);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        panic("Unable to create client socket!");
    }

    srv.sin_family      = AF_INET;
    srv.sin_port        = htons(SERVER_PORT);
    srv.sin_addr.s_addr = inet_addr(address);

    if (connect(sock, (struct sockaddr *)&srv, sizeof(srv)) >= 0) {
        worker_start();
        return 1; // success
    }

    return 0; // failure
}

void* worker() {
    int rc;
    struct timeval timeout;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 50000;

    if (pthread_detach(pthread_self()) != 0) {
        panic("Error detaching pthread!");
    }

    do {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock, &fds);
        mbuf_t mbuf;

        // send messages to the server if any
        while (mqueue_get(&c2s_queue, &mbuf) > 0) {
            if ((rc = write(sock, &mbuf.msg, sizeof(msg_t))) < 0) {
                panic("Error sending message in worker!");
            }

            if (mbuf.msg.size == 0) {
                continue;
            }

            loggerf("Writing to sock: %d", sock);
            if ((rc = write(sock, mbuf.payload, mbuf.msg.size)) < 0) {
                panic("Error sending message in worker!");
            }

            free(mbuf.payload);
        }

        do {
            rc = select(sock + 1, &fds, NULL, NULL, &timeout);
        } while (rc < 0 && errno == EINTR);

        if (rc < 0) {
            server_connected = 0;
            panic("server disconnected!");
            return NULL;
        } else if (rc == 0) {
            continue;
        }


        if ((rc = readall(sock, &mbuf.msg, sizeof(mbuf.msg))) == 0) {
            server_connected = 0;
            // TODO implement dialog with this message:
            logger("[C] Error getting message in worker!");
            continue;
        } else if (rc < 0) {
            logger("[C] Error reading from socket!");
            server_connected = 0;
            break;
        }

        // rc > 0
        switch (mbuf.msg.type) {
            case MSG_PUT_CHAT:
                logger("[C] [PUT_CHAT]");
                break;
            case MSG_PUT_SYSMSG:
                logger("[C] [PUT_SYSMSG]");
                break;
            default:
                warnf("Unknown type: %d", mbuf.msg.type);
                logger("[C] [UNKNOWN]");
                continue;
        }

        char *payload;

        if (mbuf.msg.size > 0) {
            payload = malloc(mbuf.msg.size);

            if (payload == NULL) {
                panic("Unable to allocate buffer for payload!");
            }

            if (readall(sock, payload, mbuf.msg.size) !=
                    (ssize_t)mbuf.msg.size) {
                logger("[C] Error reading payload");
            }

            loggerf("[C] Received buf: [%s]", payload);
        }

        switch (mbuf.msg.type) {
            case MSG_PUT_CHAT:
                c_chat_add(payload);

                free(payload);

                break;
            case MSG_PUT_SYSMSG:
                c_sysmsg_add(payload);

                free(payload);

                break;
            default:
                warnf("Unknown type: %d", mbuf.msg.type);
                logger("[C] [UNKNOWN]");
                continue;
        }
    } while (server_connected == 1 && ! end);

    return NULL;
}

void worker_start() {
    if (pthread_create(&thr_worker, NULL, &worker, NULL) != 0) {
        panic("Error creating worker thread!");
    }
}

int client() {  
    if (atexit(&at_exit) != 0) {
        panic("Unable to set exit handler!");
    }

    if (setlocale(LC_ALL, "ru_RU.UTF-8") == NULL ||
            setlocale(LC_ALL, "en_US.UTF-8") == NULL) {
        panic("Unable to set locale (ru,en)UTF-8!");
    }

    locale_init(CONF_SVAL("file_locale"));

    mqueue_init(&c2s_queue);

    stuff_init();

    keyboard_init();

    windows_check();

    struct sigaction sa_ignore;
    sa_ignore.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &sa_ignore, NULL);

    init_screen();

    struct sigaction sa_winch;
    sa_winch.sa_handler = &sigwinch;
    sigaction(SIGWINCH, &sa_winch, NULL);

    splash_screen();

    windows_init(0);

    // TODO rewrite this to get everything from server
    area_init();
    c_chat_init();
    c_sysmsg_init();

    sock = -1;

    menu_msg[0] = '\0';

    while (server_connected == 0) {
        menu(M_MAIN);
    }

    end = 0;
    do {

        CHECK_CONNECTION();

        windows_redraw();

        wtimeout(stdscr, 100);
        last_key = mvgetch(max_y - 1, max_x - 1);
        if (K[K_MENU_LARGE] == last_key) {
            menu(M_MAIN);
        } else if (K[K_CHAT_LARGE] == last_key) {
            c_chat_open();
        } else if (K[K_SYSMSG_LARGE] == last_key) {
            c_sysmsg_open();
        } else if (K[K_INVENTORY_LARGE] == last_key) {
            c_inventory_open();
        } else if (K[K_EXIT] == last_key) {
            end = 1;
        } else if (K[K_CLR_SCR] == last_key) {
            wclear(stdscr);
            wrefresh(stdscr);
        }
    } while (! end);

    mqueue_destroy(&c2s_queue);

    return 0;
}

int color2attr(enum colors color) {
    if (++color > 8) {
        color = COLOR_PAIR(color - 8);
        color |= A_BOLD;
    } else {
        color = COLOR_PAIR(color);
    }

    return color;
}
