// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"

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
    getmaxyx(stdscr, max_y, max_x);
}

void redraw() {
    mvwprintw(stdscr, 1, 1, "%d %d", max_y, max_x);
    box(stdscr, '|', '-');
    box(w_area, '+', '+');

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

    init_screen();

    struct sigaction sa_winch;
    sa_winch.sa_handler = &sigwinch;
    sigaction(SIGWINCH, &sa_winch, NULL);

    w_color(stdscr, LGREEN);

    if ((w_area = subwin(stdscr, 5, 20, 3, 2)) == NULL) {
        panic("Window creation failure!");
    }

    w_color(w_area, LRED);

    int ch;
    do {
        redraw();
        usleep(10000);
    } while ((ch = mvgetch(max_y - 1, max_x - 1)) != 'q');


    return 0;
}
