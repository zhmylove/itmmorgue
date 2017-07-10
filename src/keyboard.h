// vim: sw=4 ts=4 et :
#ifndef KEYBOARD_H
#define KEYBOARD_H

enum keyboard {
    K_EXIT = 0x0,
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
    K_MENU_LARGE,
    K_INVENTORY_LARGE,
    K_CHAT_LARGE,

    // Chat controls
    K_CHAT_EXIT,
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
