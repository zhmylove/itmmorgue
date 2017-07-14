// vim: sw=4 ts=4 et :
#ifndef WINDOWS_H
#define WINDOWS_H

// All windows
enum windows {
    W_STDSCR,
    W_AREA,         // that thing with the world
    W_CHAT,         // inter player communication
    W_INVENTORY,    // players bag
    W_MAP,          // stuff used to check the whole memorized world
    W_STATUS,       // place to print info about character
    W_SYSMSG,       // in-game system messages (damage, curses, ...)
    W_SIZE
};
// String representation of windows names
// Defined in windows.c
extern char *windows_names[];

// WINDOW pointer by NAME
#define W(NAME) (windows[NAME].w)
#define WIN(NAME, PROPERTY) (windows[W_##NAME].PROPERTY)

#define MVW(WIN, ...) do { mvwprintw(W(WIN), __VA_ARGS__); } while(0)

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
int max_x, max_y, focus;

void init_screen();
void sigwinch(int signum);
void windows_check();
void windows_init();
void windows_redraw();
void windows_fill();
void inventory_open();
int wcolor(WINDOW *win, int color);

#endif /* WINDOWS_H */
