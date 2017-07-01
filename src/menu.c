// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "config.h"

void main_menu(int id, WINDOW *win);

static struct menu {
    void (*f)(int id, WINDOW *);
    char *caption;
} menus[] = {
    { main_menu, "Main menu" }
};

void main_menu(int id, WINDOW *win) {
    mvwprintw(win, 0, 0, _(menus[id].caption));

    wgetch(win);

    return;
}

void menu(size_t menu_id) {
    WINDOW *w_menu;

    if (menu_id >= sizeof(menus) / sizeof(struct menu)) {
        return;
    }

    if ((w_menu = newwin(0, 0, 0, 0)) == NULL) {
        panic("Unable to create menu window!");
    }

    wtimeout(w_menu, -1);

    menus[menu_id].f(menu_id, w_menu);

    delwin(w_menu);

    return;
}
