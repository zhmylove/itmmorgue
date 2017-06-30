// vim: sw=4 ts=4 et :
#include "itmmorgue.h"

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

    return 0;
}
