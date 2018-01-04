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

    // Send nickname
    mbuf_t mbuf;
    mbuf.msg.type = MSG_REPORT_NICKNAME;
    mbuf.msg.size = strlen(CONF_SVAL("player_nickname")) + 2;
    if (NULL == (mbuf.payload = (char*)malloc(mbuf.msg.size))) {
        panic("[C] Cannot allocate nickname payload buffer");
    }
    char color[2] = "0";
    color[0] = '0' + CONF_IVAL("player_color");
    strcpy(mbuf.payload, color);
    strcat(mbuf.payload, CONF_SVAL("player_nickname"));
    mqueue_put(&c2s_queue, mbuf);

    do {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock, &fds);
        mbuf_t mbuf;

        // send messages to the server if any
        while (mqueue_get(&c2s_queue, &mbuf) > 0) {
            if (-1 == send_mbuf(sock, &mbuf)) {
                panic("Error sending message in worker!");
            }
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
            case MSG_PUT_AREA:
                logger("[C] [PUT_AREA]");
                break;
            case MSG_PUT_PLAYERS_FULL:
                logger("[C] [PUT_PLAYERS_FULL]");
                break;
            case MSG_PUT_PLAYERS:
                logger("[C] [PUT_PLAYERS]");
                break;
            case MSG_PUT_LEVEL:
                logger("[C] [PUT_LEVEL]");
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
            case MSG_PUT_AREA:
                c_area_update(1, (tileblock_t *)payload);

                free(payload);

                break;
            case MSG_PUT_PLAYERS_FULL:
                c_receive_players_full((players_full_mbuf_t *)payload);

                free(payload);

                break;
            case MSG_PUT_PLAYERS:
                c_receive_players((players_mbuf_t *)payload);

                free(payload);

                break;
            case MSG_PUT_LEVEL:
                c_level_add((level_t *)payload);

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
#define HANDLE_MOVE(move) \
        } else if (K[move] == last_key) { \
            c_send_move(move)
            HANDLE_MOVE(K_MOVE_LEFT);
            HANDLE_MOVE(K_MOVE_RIGHT);
            HANDLE_MOVE(K_MOVE_UP);
            HANDLE_MOVE(K_MOVE_DOWN);
            HANDLE_MOVE(K_MOVE_LEFT_UP);
            HANDLE_MOVE(K_MOVE_RIGHT_UP);
            HANDLE_MOVE(K_MOVE_LEFT_DOWN);
            HANDLE_MOVE(K_MOVE_RIGHT_DOWN);
#undef HANDLE_MOVE
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
