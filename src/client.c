// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "windows.h"
#include "client.h"
#include "config.h"

void at_exit(void) {
    if (! isendwin()) {
        endwin();
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

    config_init("itmmorgue.conf");

    locale_init(conf("locale_file").sval);

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

    int end = 0;
    do {
        windows_redraw();

        wtimeout(stdscr, 100);
        switch (last_key = mvgetch(max_y - 1, max_x - 1)) {
            case 'm':
                menu(M_MAIN);
                break;
            case 'c':
                chat_open();
                break;
            case 'i':
                inventory_open();
                break;
            case 'q':
            case 'Q':
                end = 1;
                break;
            case 0xC: // Control-L
                wclear(stdscr);
                wrefresh(stdscr);
                break;
            case 'l':
                windows[3].x += 1;
                break;
            case 'h':
                windows[3].x -= 1;
                break;
            case 'j':
                windows[3].y += 1;
                break;
            case 'k':
                windows[3].y -= 1;
                break;
        }
    } while (! end);

    return 0;
}
