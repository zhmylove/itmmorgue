// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "windows.h"

// String representation of windows names
char *windows_names[] = {
    "stdscr",
    "area",
    "chat",
    "inventory",
    "map",
    "status",
    "sysmsg",
    NULL
};

// Windows draw sequence
int windows_order[W_SIZE] = {
    W_STDSCR,
    W_AREA,
    W_STATUS,
    W_INVENTORY,
    W_MAP,
    W_CHAT,
    W_SYSMSG
};

void windows_check() {
    // Dynamic assertions to check compile-time error ;-)
    if (W_SIZE != sizeof(windows_names) / sizeof(char *) - 1) {
        panic("SOURCE CODE ERROR: windows inconsistency!");
    }
}

void windows_init() {
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

        FILL_WIN_PARAMETER(buf, state);

        if (windows[i].state == LARGE) {
            strncat(buf, "_large", BUFSIZ - 12);
        } else {
            strncat(buf, "_small", BUFSIZ - 12);
        }

        FILL_WIN_PARAMETER(buf, y);
        FILL_WIN_PARAMETER(buf, x);
        FILL_WIN_PARAMETER(buf, max_y);
        FILL_WIN_PARAMETER(buf, max_x);
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
}

static void resize() {
    endwin();
    init_screen();
}

void sigwinch(int signum) {
    resize();

    (void)signum;
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

    clear();
    refresh();
    windows[0].w = stdscr;
    getmaxyx(stdscr, max_y, max_x);
}

static void draw_any_before() {
    // Draw all windows
    for (size_t i = 0; i < W_SIZE; i++) {
        // Restore window size and position
        wresize(W(i), windows[i].max_y, windows[i].max_x);
        if (i != 0) {
            mvderwin(W(i), windows[i].y, windows[i].x);
        }
    }
}

static void draw_stdscr() {
    mvwprintw(stdscr, 1, 1,
            "%d x %d %s %d       ",
            max_y,
            max_x,
            _("Key:"),
            last_key);
}

static void draw_sysmsg() {
    return;
}

static void draw_map() {
    return;
}

static void draw_status() {
    return;
}

static void draw_area() {
    return;
}

static void draw_chat() {
    struct timeval time;
    gettimeofday(&time, NULL);

    MVW(W_CHAT, 0, 0, "1234567890abcdefghijklmnopqrstuvwxyz");
    wprintw(W(W_CHAT), " %d ", time.tv_sec);
    wprintw(W(W_CHAT), "ABCDEFJHIJKLMNOPQRSTUVWXYZ0123456789");
}

static void draw_inventory() {
    MVW(W_INVENTORY, 0, 0, "1234567890абвгдеёжзиклмнопрстуфхцчшщъыьэюя");
    wprintw(W(W_INVENTORY), "ABCDEFJHIJKLMNOPQRSTUVWXYZ0123456789");
}

static void draw_by_name(int win) {
    switch(win) {
        case W_STDSCR:    draw_stdscr();    break;
        case W_AREA:      draw_area();      break;
        case W_CHAT:      draw_chat();      break;
        case W_INVENTORY: draw_inventory(); break;
        case W_MAP:       draw_map();       break;
        case W_STATUS:    draw_status();    break;
        case W_SYSMSG:    draw_sysmsg();    break;
        default: panic("Invalid window specified for draw()!");
    }
}

static void draw_any(int win) {
    // Draw borders
    wborder(W(win), '|', '|', '-', '-', '+', '+', '+', '+');
}

void windows_redraw() {
    if (max_x < 80 || max_y < 24) {
        redrawwin(stdscr);
        mvwprintw(stdscr, 
                (int)(max_y / 2) - 2,
                (int)(max_x / 2) - 4,
                "ITMMORGUE"
                );
        mvwprintw(stdscr, 
                (int)(max_y / 2) - 1,
                (int)(max_x / 2) - 15,
                "Windows is smaller than 80x24!"
                );
        wrefresh(stdscr);
        return;
    }

    draw_any_before();

    for (int i = 0; i < W_SIZE; i++) {
        if (windows[i].state == HIDDEN) {
            continue;
        }

        // TODO check if we really need this heavy stuff.
        // Also we can call wclear() only for each visibility change
        //wclear(W(i));

        draw_any(windows_order[i]);
        draw_by_name(windows_order[i]);
    }

    refresh();
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

