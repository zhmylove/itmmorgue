// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "windows.h"
#include "area.h"
#include "chat.h"
#include "keyboard.h"

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

void windows_colors() {
    wcolor(W(W_STDSCR),    D_WHITE);
    wcolor(W(W_AREA),      D_WHITE);
    wcolor(W(W_CHAT),      D_YELLOW);
    wcolor(W(W_INVENTORY), D_RED);
    wcolor(W(W_MAP),       D_BLUE);
    wcolor(W(W_STATUS),    D_CYAN);
    wcolor(W(W_SYSMSG),    D_MAGENTA);
}

static void windows_clearall() {
    for (int i = 0; i < W_SIZE; i++) {
        wclear(W(i));
    }
}

void windows_fill(int win, int keep_state) {
    char buf[BUFSIZ]; // for config parameter prefix
    strncpy(buf, "win_", 5);                      // buf  = "win_"
    strncat(buf, windows_names[win], BUFSIZ - 5); // buf  = "win_area"

#define FILL_WIN_PARAMETER(prefix, parameter)                                \
    /* Such small macro definitions inspired by FreeBSD brilliant code */    \
    do { char buf2[BUFSIZ]; /* for full config parameter */                  \
        strcpy(buf2, prefix);                                                \
        strncat(buf2, "_"#parameter, BUFSIZ - strlen(buf));                  \
        char buf3[BUFSIZ];                                                   \
        strcpy(buf3, buf2);                                                  \
        strncat(buf3, "_ispercent", BUFSIZ - strlen(buf2));                  \
        int val = conf(buf2).ival;                                           \
        if ((char*)#parameter == (char*)"max_y") {                           \
            int ispercent = conf(buf3).ival;                                 \
            if (ispercent) val = (int)(max_y * val / 100);                   \
            windows[win].parameter =                                         \
            val < 0 ? max_y + val - windows[win].y : val;                    \
        } else if ((char*)#parameter == (char*)"max_x") {                    \
            int ispercent = conf(buf3).ival;                                 \
            if (ispercent) val = (int)(max_x * val / 100);                   \
            windows[win].parameter =                                         \
            val < 0 ? max_x + val - windows[win].x : val;                    \
        } else if ((char*)#parameter == (char*)"y") {                        \
            int ispercent = conf(buf3).ival;                                 \
            if (ispercent) val = (int)(max_y * val / 100);                   \
            windows[win].parameter = val < 0 ? max_y + val : val;            \
        } else if ((char*)#parameter == (char*)"x") {                        \
            int ispercent = conf(buf3).ival;                                 \
            if (ispercent) val = (int)(max_x * val / 100);                   \
            windows[win].parameter = val < 0 ? max_x + val : val;            \
        } else {                                                             \
            windows[win].parameter = val;                                    \
        }                                                                    \
    } while(0)

    if (keep_state == 0) {
        FILL_WIN_PARAMETER(buf, state);
    }

    if (windows[win].state == LARGE) {
        strncat(buf, "_large", BUFSIZ - 12);
    } else {
        strncat(buf, "_small", BUFSIZ - 12);
    }

    FILL_WIN_PARAMETER(buf, y);
    FILL_WIN_PARAMETER(buf, x);
    FILL_WIN_PARAMETER(buf, max_y);
    FILL_WIN_PARAMETER(buf, max_x);
#undef FILL_WIN_PARAMETER
}

void windows_init() {
    focus = W_AREA;

    for (int i = 0; i < W_SIZE; i++) {
        windows_fill(i, 0);

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

    windows_clearall();
    windows_colors();
}

static void resize() {
    endwin();
    init_screen();
    windows_init();
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
        if (windows[i].max_y == 0) windows[i].max_y = max_y;
        if (windows[i].max_x == 0) windows[i].max_x = max_x;

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
    struct timeval time;
    gettimeofday(&time, NULL);

    MVW(W_SYSMSG, 1, 1, "%d ", time.tv_sec);
    return;
}

static void draw_map() {
    return;
}

static void draw_status() {
    return;
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
    mvwprintw(windows[win].w, 0, 1, "%s", windows_names[win]);
}

void windows_redraw() {
    if (max_x < 80 || max_y < 25) {
        redrawwin(stdscr);
        mvwprintw(stdscr, 
                (int)(max_y / 2) - 2,
                (int)(max_x / 2) - 4,
                "ITMMORGUE"
                );
        mvwprintw(stdscr, 
                (int)(max_y / 2) - 1,
                (int)(max_x / 2) - 15,
                "Windows is smaller than 80x25!"
                );
        wrefresh(stdscr);
        return;
    }

    draw_any_before();

    for (int i = 0; i < W_SIZE; i++) {
        if (windows_order[i] == focus ||
                windows[windows_order[i]].state == HIDDEN) {
            continue;
        }

        // TODO check if we really need this heavy stuff.
        // Also we can call wclear() only for each visibility change
        //wclear(W(windows_order[i]));

        draw_any(windows_order[i]);
        draw_by_name(windows_order[i]);
    }

    werase(W(focus));
    draw_any(focus);
    draw_by_name(focus);

    refresh();
}

int wcolor(WINDOW *win, int color) {
    int color_curses = color2attr(color);

    return wattrset(win, color_curses);
}

void inventory_open() {
    int focus_old = focus;

    windows[W_INVENTORY].state = LARGE;
    focus = W_INVENTORY;
    windows_fill(W_INVENTORY, 1);

    windows_redraw();

    wtimeout(W(W_INVENTORY), -1);
    do {
        switch (last_key = mvwgetch(W(W_INVENTORY), 0, 0)) {
            case '1':
                warn("1");
            case '2':
                warn("2");
                break;
        }
    } while (last_key != K[K_EXIT]);

    windows[W_INVENTORY].state = HIDDEN;
    focus = focus_old;
    windows_fill(W_INVENTORY, 1);
}

