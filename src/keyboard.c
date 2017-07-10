// vim: sw=4 ts=4 et :
#include "keyboard.h"
#include "config.h"

void keyboard_init() {
    K[K_EXIT]             = conf("key_exit").ival;
    K[K_CLR_SCR]          = conf("key_clear_screen").ival;
    K[K_BACKSPACE]        = conf("key_backspace").ival;

    // Movement
    K[K_MOVE_LEFT]        = conf("key_move_left").ival;
    K[K_MOVE_RIGHT]       = conf("key_move_right").ival;
    K[K_MOVE_UP]          = conf("key_move_up").ival;
    K[K_MOVE_DOWN]        = conf("key_move_down").ival;
    K[K_MOVE_LEFT_UP]     = conf("key_move_left_up").ival;
    K[K_MOVE_RIGHT_UP]    = conf("key_move_right_up").ival;
    K[K_MOVE_LEFT_DOWN]   = conf("key_move_left_down").ival;
    K[K_MOVE_RIGHT_DOWN]  = conf("key_move_right_down").ival;

    // Window control
    K[K_MENU_LARGE]       = conf("key_menu_large").ival;
    K[K_INVENTORY_LARGE]  = conf("key_inventory_large").ival;
    K[K_CHAT_LARGE]       = conf("key_chat_large").ival;

    // Chat controls
    K[K_CHAT_EXIT]        = conf("key_chat_exit").ival;
    K[K_CHAT_SEND]        = conf("key_chat_send").ival;

    // Others
    K[K_ZERO]             = conf("key_zero").ival;
    K[K_ONE]              = conf("key_one").ival;
    K[K_TWO]              = conf("key_two").ival;
    K[K_THREE]            = conf("key_three").ival;
    K[K_FOUR]             = conf("key_four").ival;
    K[K_FIVE]             = conf("key_five").ival;
    K[K_SIX]              = conf("key_six").ival;
    K[K_SEVEN]            = conf("key_seven").ival;
    K[K_EIGHT]            = conf("key_eight").ival;
    K[K_NINE]             = conf("key_nine").ival;
}
