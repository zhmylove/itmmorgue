// vim: sw=4 ts=4 et :
#ifndef KEYBOARD_H
#define KEYBOARD_H

/*
 * Client's key bindings.
 *
 * Items of this enum are indexes of K[], which represents key map. To add a
 * key, put its name into this enum, add configuration parameter into
 * default_config.h and assign it to corresponding element of K[] in keyboard.c
 * by calling conf("key_name").value.__cval.
 */
enum keyboard {
    K_EXIT = 0,
    K_WINDOW_EXIT,
    K_CLR_SCR,
    K_BACKSPACE,

    // Movement
    K_MOVE_LEFT,
    K_MOVE_RIGHT,
    K_MOVE_UP,
    K_MOVE_DOWN,
    K_MOVE_LEFT_UP,
    K_MOVE_RIGHT_UP,
    K_MOVE_LEFT_DOWN,
    K_MOVE_RIGHT_DOWN,

    // Window control
    K_SCROLL_UP,
    K_SCROLL_DOWN,
    K_MENU_LARGE,
    K_INVENTORY_LARGE,
    K_CHAT_LARGE,
    K_SYSMSG_LARGE,

    // Chat controls
    K_CHAT_SEND,

    // Others
    K_ZERO,
    K_ONE,
    K_TWO,
    K_THREE,
    K_FOUR,
    K_FIVE,
    K_SIX,
    K_SEVEN,
    K_EIGHT,
    K_NINE,

    K_SIZE
};

int K[K_SIZE];

void keyboard_init();

#endif /* KEYBOARD_H */
