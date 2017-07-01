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

void splash_screen() {
    WINDOW *splash;
    if ((splash = newwin(0, 0, 0, 0)) == NULL) {
        panic("Unable to create splash window!");
    }
    nodelay(splash, FALSE);

    for (int i = max_y - 1; i >= 0; i--) {
        w_color(splash, 8 + i % 8);
        mvwprintw(splash, i, 0, ".");
        for (int j = 1; j < max_x; j++) {
            wprintw(splash, ".");
        }
        wrefresh(splash);
        usleep(20000);
    }

    werase(splash);
    wrefresh(splash);

    int splash_delay = conf("splash_delay").ival;

#define s(y1, x1, c1, y2, x2, c2)  \
    w_color(splash, L_RED);        \
    mvwprintw(splash, y1, x1, c1); \
    w_color(splash, L_BLUE);       \
    mvwprintw(splash, y2, x2, c2); \
    wrefresh(splash);              \
    usleep(splash_delay);

    s(    7, 35, "-",   7, 40, "-");
    s(    7, 34, "-",   7, 41, "-");
    s(    7, 33, "-",   7, 42, "-");
    s(    7, 32, "-",   8, 42, "|");
    s(    8, 32, "|",   9, 42, "|");
    s(    9, 32, "|",  10, 42, "|");
    s(   10, 32, "|",  11, 42, "|");
    s(   11, 32, "|",  12, 42, "|");
    s(   12, 32, "|",  13, 42, "|");
    s(   13, 32, "|",  14, 42, "|");
    s(   14, 32, "|",  15, 42, "-");
    s(   15, 32, "-",  15, 43, "-");
    s(   15, 31, "-",  15, 44, "-");
    s(   15, 30, "-",  14, 44, "|");
    s(   14, 30, "|",  13, 44, "|");
    s(   13, 30, "|",  12, 44, "|");
    s(   12, 30, "|",  11, 44, "|");
    s(   11, 30, "|",  11, 45, "\\"); //o
    s(   11, 29, "/",  12, 46, "\\");
    s(   11, 28, "\\", 13, 47, "|");
    s(   11, 27, "|",  14, 47, "|");
    s(   12, 27, "|",  15, 47, "-"); //o
    s(   13, 27, "|",  15, 48, "-");
    s(   14, 27, "|",  15, 49, "-");
    s(   15, 27, "-",  14, 49, "|");
    s(   15, 26, "-",  13, 49, "|");
    s(   15, 25, "-",  12, 48, "\\");
    s(   14, 25, "|",  11, 48, "/");
    s(   13, 25, "|",  10, 49, "/");
    s(   12, 25, "|",   9, 49, "\\"); //o
    s(   11, 25, "|",   9, 48, "-");
    s(   10, 25, "|",  10, 47, "/");
    s(    9, 25, "|",  10, 46, "\\");
    s(    8, 25, "|",   9, 46, "/");
    s(    7, 25, "-",   9, 47, "\\");
    s(    7, 24, "-",   9, 48, "="); //O
    s(    7, 23, "-",   8, 48, "\\");
    s(    8, 23, "|",   7, 48, "-");
    s(    9, 23, "|",   7, 49, "-");
    s(   10, 23, "|",   7, 50, "-");
    s(   11, 23, "|",   7, 51, "-");
    s(   12, 23, "|",   7, 52, "-");
    s(   13, 23, "|",   8, 52, "/");
    s(   14, 23, "|",   9, 52, "|"); //o
    s(   15, 23, "-",  10, 52, "|");
    s(   15, 22, "-",  11, 52, "|");
    s(   15, 21, "-",  12, 52, "|");
    s(   14, 21, "|",  13, 52, "|");
    s(   13, 21, "|",  14, 52, "\\");
    s(   12, 21, "|",  15, 53, "-");
    s(   11, 21, "|",  15, 54, "-");
    s(   11, 20, "/",  15, 55, "-");
    s(   11, 19, "\\", 15, 56, "-");
    s(   11, 18, "|",  15, 57, "-");
    s(   12, 18, "|",  14, 58, "/");
    s(   13, 18, "|",  13, 58, "|");
    s(   14, 18, "|",  12, 58, "|"); //o
    s(   15, 18, "-",  11, 57, "_");
    s(   15, 17, "-",  11, 56, "_");
    s(   15, 16, "-",  12, 55, "|");
    s(   14, 16, "|",  12, 56, "_");
    s(   13, 16, "|",  13, 56, "|");
    s(   12, 16, "|",  13, 55, "_");
    s(   11, 16, "|",  13, 54, "|");
    s(   10, 16, "|",  12, 54, "|");
    s(    9, 16, "|",  11, 54, "|");
    s(    8, 16, "|",  10, 54, "|");
    s(    7, 16, "-",   9, 54, "|"); //o
    s(    7, 15, "-",   8, 55, "_");
    s(    8, 14, "/",   9, 56, "|");
    s(    8, 13, "_",   9, 57, "_");
    s(    9, 12, "|",   9, 58, "|");
    s(   10, 12, "|",   8, 58, "\\");
    s(   11, 12, "|",   7, 58, "-");
    s(   12, 12, "|",   7, 59, "-");
    s(   13, 12, "|",   7, 60, "-");
    s(   14, 12, "|",   7, 61, "-");
    s(   15, 12, "-",   8, 60, "/");
    s(   15, 11, "-",   9, 60, "|"); //o
    s(   15, 10, "-",  10, 60, "|");
    s(   14, 10, "|",  11, 60, "|");
    s(   13, 10, "|",  12, 60, "|");
    s(   12, 10, "|",  13, 60, "|");
    s(   11, 10, "|",  14, 60, "\\");
    s(   10, 10, "|",  15, 61, "-"); //o
    s(    9, 10, "|",  15, 62, "-");
    s(    8, 9, "_",   15, 63, "-");
    s(    8, 8, "\\",  15, 64, "-");
    s(    7, 7, "-",   15, 65, "-");
    s(    8, 6, "/",   14, 66, "/"); //o
    s(    9, 5, "|",   13, 66, "|");
    s(   10, 5, "|",   12, 66, "|");
    s(   11, 5, "|",   11, 66, "|");
    s(   12, 5, "|",   10, 66, "|");
    s(   13, 5, "|",    9, 66, "|");
    s(   14, 6, "\\",   8, 66, "\\");
    s(   15, 6, "-",    7, 65, "-");
    s(   15, 5, "-",    7, 66, "-");
    s(   15, 4, "-",    7, 67, "-");
    s(   15, 3, "-",    7, 68, "-");
    s(   15, 2, "-",    8, 68, "|"); //o
    s(   14, 2, "/",    9, 68, "|");
    s(   13, 3, "|",   10, 68, "|");
    s(   12, 3, "|",   11, 68, "|");
    s(   11, 3, "|",   12, 68, "|");
    s(   10, 3, "|",   13, 68, "|");
    s(    9, 3, "|",   14, 68, "|");
    s(    8, 2,"\\",   15, 68, "-");
    s(    7, 2, "-",   15, 69, "-");
    s(    7, 3, "-",   15, 70, "-");
    s(    7, 4, "-",   15, 71, "-");
    s(    7, 5, "-",   15, 72, "-");
    s(    7, 6, "-",   15, 73, "-");
    s(    7, 7, "=",   15, 74, "-"); //o
    s(    7, 8, "-",   14, 74, "\\");
    s(    7, 9, "-",   13, 73, "_");
    s(    7, 10, "-",  13, 72, "_");
    s(    7, 11, "-",  13, 71, "_");
    s(    7, 12, "-",  13, 70, "|");
    s(    7, 13, "-",  12, 70, "|");
    s(    7, 14, "-",  11, 71, "_");
    s(    7, 15, "=",  11, 72, "_");
    s(    7, 16, "-",  11, 73, "|");
    s(    7, 17, "-",  10, 72, "_"); //O
    s(    8, 18, "\\", 10, 71, "_");
    s(    9, 19, "\\", 10, 70, "|");
    s(    9, 20, "/",   9, 70, "|");
    s(    8, 21, "/",   8, 71, "_");
    s(    7, 22, "-",   8, 72, "_");
    s(    7, 23, "-",   8, 73, "_");
    s(    7, 24, "=",   8, 74, "/");
    s(    7, 25, "-",   7, 74, "-");
    s(    7, 26, "-",   7, 73, "-");
    s(    8, 27, "\\" , 7, 72, "-");
    s(    9, 28, "\\" , 7, 71, "-");
    s(    9, 29, "/",   7, 70, "-");
    s(    8, 30, "/",   7, 69, "-");
    s(    7, 31, "-",   7, 68, "-");
    s(    7, 32, "-",   7, 67, "=");
    s(    7, 33, "=",   7, 66, "=");
    s(    7, 34, "=",   7, 65, "=");
    s(    7, 35, "=",   8, 64, "/");
    s(    8, 34, "/",   9, 64, "|");
    s(    9, 34, "|",  10, 64, "|");
    s(   10, 34, "|",  11, 64, "|");
    s(   11, 34, "|",  12, 64, "|");
    s(   11, 35, "-",  13, 64, "|");
    s(   12, 36, "|",  13, 63, "_");
    s(   13, 36, "|",  13, 62, "|");
    s(   13, 37, "_",  12, 62, "|");
    s(   13, 38, "|",  11, 62, "|");
    s(   12, 38, "|",  10, 62, "|");
    s(   11, 38, "|",   9, 62, "|");
    s(   10, 38, "|",   8, 62, "\\"); //o
    s(    9, 38, "|",   7, 61, "=");
    s(    8, 37, "_",   7, 60, "=");
    s(    9, 36, "|",   7, 59, "=");
    s(   10, 36, "|",   7, 58, "=");
    s(   11, 36, "|",   7, 57, "-");
    s(   11, 35, "=",   7, 56, "-");
    s(   12, 34, "|",   7, 55, "-");
    s(   13, 34, "|",   7, 54, "-");
    s(   14, 34, "\\",  7, 53, "-");
    s(   15, 35, "-",   7, 52, "=");
    s(   15, 36, "-",   7, 51, "=");
    s(   15, 37, "-",   7, 50, "=");
    s(   15, 38, "-",   7, 49, "=");
    s(   15, 39, "-",   7, 48, "=");
    s(   14, 40, "/",   7, 47, "-");
    s(   13, 40, "|",   7, 46, "-");
    s(   12, 40, "|",   8, 45, "/");
    s(   11, 40, "|",   8, 44, "\\");
    s(   10, 40, "|",   7, 43, "-");
    s(    9, 40, "|",   7, 42, "=");
    s(    8, 40, "\\",  7, 41, "=");
    s(    7, 39, "-",   7, 40, "@");
    s(    7, 38, "-",   7, 40, "@");
    s(    7, 37, "-",   7, 40, "@");
    s(    7, 36, "-",   7, 40, "@");
    s(    7, 36, "=",   7, 40, "@");
    s(    7, 37, "=",   7, 40, "@");
    s(    7, 38, "=",   7, 40, "@");
    s(    7, 39, "@",   7, 40, "@");

#undef s

    usleep(70000000);

    delwin(splash);
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

    splash_screen();

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
