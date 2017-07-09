// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "windows.h"
#include "config.h"
#include "stuff.h"
#include "keyboard.h"

#include "area.h"
#include "chat.h"

void at_exit(void) {
    if (! isendwin()) {
        endwin();
    }

    if (server_started) {
        kill(server_started, SIGTERM);
    }
}

int connect_to_server(char *address) {
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        panic("Unable to create client socket!");
    }

    srv.sin_len         = sizeof(srv);
    srv.sin_family      = AF_INET;
    srv.sin_port        = htons(SERVER_PORT);
    srv.sin_addr.s_addr = inet_addr(address);

    if (connect(sock, (struct sockaddr *)&srv, sizeof(srv)) >= 0) {
        return 1; // success
    }

    return 0; // failure
}

int client() {  
    if (atexit(&at_exit) != 0) {
        panic("Unable to set exit handler!");
    }

    if (setlocale(LC_ALL, "ru_RU.UTF-8") == NULL ||
            setlocale(LC_ALL, "en_US.UTF-8") == NULL) {
        panic("Unable to set locale (ru,en)UTF-8!");
    }

    locale_init(conf("locale_file").sval);

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

    windows_init();

    // TODO rewrite this to get everything from server
    area_init();
    chat_init();

    if (server_connected == 0) {
        menu(M_MAIN);
    }

    int end = 0;
    do {
        windows_redraw();

        wtimeout(stdscr, 100);
        last_key = mvgetch(max_y - 1, max_x - 1);
        if (K[K_MENU_LARGE] == last_key) {
            menu(M_MAIN);
        } else if (K[K_CHAT_LARGE] == last_key) {
            chat_open();
        } else if (K[K_INVENTORY_LARGE] == last_key) {
            inventory_open();
        } else if (K[K_EXIT] == last_key) {
            end = 1;
        } else if (K[K_CLR_SCR] == last_key) {
            wclear(stdscr);
            wrefresh(stdscr);
        }
    } while (! end);

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
