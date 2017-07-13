// vim: sw=4 ts=4 et :
#include "keyboard.h"
#include "config.h"

/*
 * Initializes keyboard by configuration values
 */
void keyboard_init() {
    K[K_EXIT]             = conf("key_exit").cval;
    K[K_CLR_SCR]          = conf("key_clear_screen").cval;
    K[K_BACKSPACE]        = conf("key_backspace").cval;

    // Movement
    K[K_MOVE_LEFT]        = conf("key_move_left").cval;
    K[K_MOVE_RIGHT]       = conf("key_move_right").cval;
    K[K_MOVE_UP]          = conf("key_move_up").cval;
    K[K_MOVE_DOWN]        = conf("key_move_down").cval;
    K[K_MOVE_LEFT_UP]     = conf("key_move_left_up").cval;
    K[K_MOVE_RIGHT_UP]    = conf("key_move_right_up").cval;
    K[K_MOVE_LEFT_DOWN]   = conf("key_move_left_down").cval;
    K[K_MOVE_RIGHT_DOWN]  = conf("key_move_right_down").cval;

    // Window control
    K[K_MENU_LARGE]       = conf("key_menu_large").cval;
    K[K_INVENTORY_LARGE]  = conf("key_inventory_large").cval;
    K[K_CHAT_LARGE]       = conf("key_chat_large").cval;
    K[K_SYSMSG_LARGE]     = conf("key_sysmsg_large").cval;

    // Chat controls
    K[K_CHAT_EXIT]        = conf("key_chat_exit").cval;
    K[K_CHAT_SEND]        = conf("key_chat_send").cval;

    // Others
    K[K_ZERO]             = conf("key_zero").cval;
    K[K_ONE]              = conf("key_one").cval;
    K[K_TWO]              = conf("key_two").cval;
    K[K_THREE]            = conf("key_three").cval;
    K[K_FOUR]             = conf("key_four").cval;
    K[K_FIVE]             = conf("key_five").cval;
    K[K_SIX]              = conf("key_six").cval;
    K[K_SEVEN]            = conf("key_seven").cval;
    K[K_EIGHT]            = conf("key_eight").cval;
    K[K_NINE]             = conf("key_nine").cval;
}
