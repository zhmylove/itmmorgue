// vim: sw=4 ts=4 et :
#include "keyboard.h"

void keyboard_init() {
    K[K_EXIT]             = 'q';
    K[K_CLR_SCR]          = 0x0C;
    K[K_BACKSPACE]        = 0x7F;

    // Movement
    K[K_MOVE_LEFT]        = '4';
    K[K_MOVE_RIGHT]       = '6';
    K[K_MOVE_UP]          = '8';
    K[K_MOVE_DOWN]        = '2';
    K[K_MOVE_LEFT_UP]     = '4';
    K[K_MOVE_RIGHT_UP]    = '6';
    K[K_MOVE_LEFT_DOWN]   = '8';
    K[K_MOVE_RIGHT_DOWN]  = '2';

    // Window control
    K[K_MENU_LARGE]       = 'm';
    K[K_INVENTORY_LARGE]  = 'i';
    K[K_CHAT_LARGE]       = 'c';

    // Others
    K[K_ZERO]             = '0';
    K[K_ONE]              = '1';
    K[K_TWO]              = '2';
    K[K_THREE]            = '3';
    K[K_FOUR]             = '4';
    K[K_FIVE]             = '5';
    K[K_SIX]              = '6';
    K[K_SEVEN]            = '7';
    K[K_EIGHT]            = '8';
    K[K_NINE]             = '9';
}
