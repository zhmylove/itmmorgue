// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "config.h"

void at_exit(void) {
    if (! isendwin()) {
        endwin();
    }
}

int w_color(WINDOW *win, int color) {
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
    nodelay(stdscr, FALSE);
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
    mvwprintw(stdscr, 1, 1, "%d %d", max_y, max_x);

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

    w_color(W(2), D_BLUE);
    w_color(W(3), D_YELLOW);
    mvwprintw(W(2), 0, 0, "1234567890abcdefghijklmnopqrstuvwxyzABCDEFJHIJKLMNOPQRSTUVWXYZ0123456789");
    mvwprintw(W(3), 0, 0, "1234567890abcdefghijklmnopqrstuvwxyzABCDEFJHIJKLMNOPQRSTUVWXYZ0123456789");

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

    // Dynamic assertions to check compile-time error ;-)
    if (W_SIZE != sizeof(windows_names) / sizeof(char *) - 1) {
        panic("SOURCE CODE ERROR: windows inconsistency!");
    }

    init_screen();

    struct sigaction sa_winch;
    sa_winch.sa_handler = &sigwinch;
    sigaction(SIGWINCH, &sa_winch, NULL);

    w_color(stdscr, L_GREEN);

    char buf[BUFSIZ]; // for config parameter prefix
    for (int i = 0; i < W_SIZE; i++) {
        strncpy(buf, "win_", 5);                    // buf  = "win_"
        strncat(buf, windows_names[i], BUFSIZ - 5); // buf  = "win_area"

#define FILL_WIN_PARAMETER(prefix, parameter)                   \
        do { char buf2[BUFSIZ]; /* for full config parameter */ \
            strcpy(buf2, prefix);                               \
            strncat(buf2, "_"#parameter, BUFSIZ - strlen(buf)); \
            windows[i].parameter = conf(buf2).ival;             \
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

    w_color(W(W_AREA), L_RED);

    int ch;
    do {
        redraw();
        usleep(10000);
    } while ((ch = mvgetch(max_y - 1, max_x - 1)) != 'q');


    return 0;
}
