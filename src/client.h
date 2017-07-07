// vim: sw=4 ts=4 et :
#ifndef CLIENT_H
#define CLIENT_H

#include "windows.h"

int last_key;

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

typedef struct menu {
    void (*f)(int id, WINDOW *);
    char *caption;
} menu_t;

int client();

extern void menu(size_t menu_id);
extern void splash_screen();
extern void locale_init(char *file);
extern char *_(char*);

#endif /* CLIENT_H */
