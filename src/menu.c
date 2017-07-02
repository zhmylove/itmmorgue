// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "config.h"

void m_main(int id, WINDOW *win);

void m_null() {
    return;
}

void m_exit_game() {
    exit(0);
    return;
}

menu_t menus[] = {
    { m_main, "Main menu" }
};

void m_main(int id, WINDOW *win) {
    // TODO implement functions for stuff
    menu_t items[] = {
        { m_null, "New game" },
        { m_null, "Connect to existing game" },
        { m_null, "Options" },
        { m_null, "Help" },
        { m_exit_game, "Exit to Windows" }
    };

    size_t items_len = sizeof(items) / sizeof(menu_t);

    char *caption = _(menus[id].caption);
    size_t caption_len = anystrlen(caption);
    mvwprintw(win,
            max_y / 2 - items_len - 3,
            max_x / 2 - caption_len / 2,
            caption);
    for (size_t i = 0; i < caption_len; i++) {
        mvwprintw(win,
                max_y / 2 - items_len - 2,
                max_x / 2 - caption_len / 2 + i,
                "=");
    }

    for (size_t i = 0; i < items_len; i++) {
        char *item = _(items[i].caption);
        mvwprintw(win,
                max_y / 2 - items_len + i,
                max_x / 2 - anystrlen(item) / 2 - 2,
                "%d. %s",
                i + 1,
                item);
    }

    do {
        // TODO check if we wanna menus with 10+ elements
        last_key = wgetch(win);
        if (last_key > '0' && last_key <= (int)('0' + items_len)) {
            int item_id = last_key - '0' - 1;

            items[item_id].f(item_id, win);
            return;
        }
    } while (last_key != ERR);

    return;
}

void menu(size_t menu_id) {
    WINDOW *w_menu;

    if (menu_id >= sizeof(menus) / sizeof(menu_t)) {
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
