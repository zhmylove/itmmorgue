// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "config.h"

// String representation of windows names
char *windows_names[] = {
    "stdscr",
    "area",
    "chat",
    "inventory",
    NULL
};

void at_exit(void) {
    if (! isendwin()) {
        endwin();
    }
}

int wcolor(WINDOW *win, int color) {
    if (++color > 8) {
        color = COLOR_PAIR(color - 8);
        color |= A_BOLD;
    } else {
        color = COLOR_PAIR(color);
    }

    return wattrset(win, color);
}

void init_screen() {
    initscr();

    start_color();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_BLACK, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);
    init_pair(8, COLOR_MAGENTA, COLOR_BLACK);

    erase();
    refresh();
    windows[0].w = stdscr;
    getmaxyx(stdscr, max_y, max_x);
}

void redraw() {
    mvwprintw(stdscr, 1, 1,
            "%d x %d %s %d       ",
            max_y,
            max_x,
            _("Key:"),
            last_key);

    // Draw all windows
    for (size_t i = 0; i < W_SIZE; i++) {
        // Restore window size and position
        wresize(W(i), windows[i].max_y, windows[i].max_x);
        if (i != 0) {
            mvderwin(W(i), windows[i].y, windows[i].x);
        }

        // Draw borders
        box(W(i), '|', '-');
    }

    wcolor(W(2), D_BLUE);
    wcolor(W(3), D_YELLOW);
    mvwprintw(W(2), 0, 0, "1234567890abcdefghijklmnopqrstuvwxyz");
    wprintw(W(2), "ABCDEFJHIJKLMNOPQRSTUVWXYZ0123456789");
    mvwprintw(W(3), 0, 0, "1234567890абвгдеёжзиклмнопрстуфхцчшщъыьэюя");
    wprintw(W(3), "ABCDEFJHIJKLMNOPQRSTUVWXYZ0123456789");

    refresh();
}

void resize() {
    endwin();
    init_screen();
    redraw();
}

void sigwinch(int signum) {
    resize();

    (void)signum;
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

    // Dynamic assertions to check compile-time error ;-)
    if (W_SIZE != sizeof(windows_names) / sizeof(char *) - 1) {
        panic("SOURCE CODE ERROR: windows inconsistency!");
    }

    struct sigaction sa_ignore;
    sa_ignore.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &sa_ignore, NULL);

    init_screen();

    struct sigaction sa_winch;
    sa_winch.sa_handler = &sigwinch;
    sigaction(SIGWINCH, &sa_winch, NULL);

    splash_screen();

    wcolor(stdscr, L_GREEN);

    char buf[BUFSIZ]; // for config parameter prefix
    for (int i = 0; i < W_SIZE; i++) {
        strncpy(buf, "win_", 5);                    // buf  = "win_"
        strncat(buf, windows_names[i], BUFSIZ - 5); // buf  = "win_area"

#define FILL_WIN_PARAMETER(prefix, parameter)                      \
        do { char buf2[BUFSIZ]; /* for full config parameter */    \
            strcpy(buf2, prefix);                                  \
            strncat(buf2, "_"#parameter, BUFSIZ - anystrlen(buf)); \
            windows[i].parameter = conf(buf2).ival;                \
        } while(0);

        FILL_WIN_PARAMETER(buf, y);
        FILL_WIN_PARAMETER(buf, x);
        FILL_WIN_PARAMETER(buf, max_y);
        FILL_WIN_PARAMETER(buf, max_x);
        FILL_WIN_PARAMETER(buf, state);
#undef FILL_WIN_PARAMETER

        if (i == 0) {
            windows[0].w = stdscr;
            continue;
        }

        if ((W(i) = subwin(stdscr,
                        windows[i].max_y,
                        windows[i].max_x,
                        windows[i].y,
                        windows[i].x
                        )) == NULL) {
            panic("Window creation failure!");
        }
    }

    wcolor(W(W_AREA), L_RED);

    int end = 0;
    do {
        redraw();

        wtimeout(stdscr, 100);
        switch (last_key = mvgetch(max_y - 1, max_x - 1)) {
            case 'm':
                menu(M_MAIN);
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
                windows[2].x += 1;
                break;
            case 'h':
                windows[2].x -= 1;
                break;
            case 'j':
                windows[2].y += 1;
                break;
            case 'k':
                windows[2].y -= 1;
                break;
        }
    } while (! end);


    return 0;
}
