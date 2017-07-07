// vim: sw=4 ts=4 et :
#ifndef WINDOWS_H
#define WINDOWS_H

// All windows
enum windows {
    W_STDSCR,
    W_AREA,
    W_CHAT,
    W_INVENTORY,
    W_SIZE
};
// String representation of their names
// Defined in windows.c
extern char *windows_names[];

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

win_info_t windows[W_SIZE];
int max_x, max_y;

void init_screen();
void resize();
void sigwinch(int signum);
int wcolor(WINDOW *win, int color);
void windows_check();
void windows_init();
int wcolor(WINDOW *win, int color);
void redraw();
void init_screen();
void sigwinch(int signum);
void resize();

#endif /* WINDOWS_H */
