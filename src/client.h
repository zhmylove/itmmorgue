// vim: sw=4 ts=4 et :
#ifndef CLIENT_H
#define CLIENT_H

int max_x, max_y, last_key;

// Menus
enum menus {
    M_MAIN,
    M_SIZE
};

// Dark and light colors for wcolor()
enum colors {
    D_WHITE, D_YELLOW, D_RED, D_GREEN, D_BLUE, D_BLACK, D_CYAN, D_MAGENTA,
    L_WHITE, L_YELLOW, L_RED, L_GREEN, L_BLUE, L_BLACK, L_CYAN, L_MAGENTA
};

// All windows
enum windows {
    W_STDSCR,
    W_AREA,
    W_CHAT,
    W_INVENTORY,
    W_SIZE
};
static char *windows_names[] = {
    "stdscr",
    "area",
    "chat",
    "inventory",
    NULL
};

// WINDOW pointer by NAME
#define W(NAME) (windows[NAME].w)

typedef struct win_info {
    WINDOW *w;  // pointer to ncurses
    int y;      // left top corner
    int x;      // 
    int max_y;  // size
    int max_x;  //
    enum {      // state
        HIDDEN,
        SMALL,
        LARGE
    } state;
} win_info_t;

typedef struct menu {
    void (*f)(int id, WINDOW *);
    char *caption;
} menu_t;

void init_screen();
void resize();
void sigwinch(int signum);
int wcolor(WINDOW *win, int color);
int client();

extern void menu(size_t menu_id);
extern void splash_screen();
extern void i18n_init(char *file);
extern char *_(char*);

win_info_t windows[W_SIZE];

#endif /* CLIENT_H */
