// vim: sw=4 ts=4 et :
#include "keyboard.h"
#include "config.h"

/*
 * Initializes keyboard by configuration values
 */
void keyboard_init() {
    K[K_EXIT]             = CONF_CVAL("key_exit");
    K[K_WINDOW_EXIT]      = CONF_CVAL("key_window_exit");
    K[K_CLR_SCR]          = CONF_CVAL("key_clear_screen");
    K[K_BACKSPACE]        = CONF_CVAL("key_backspace");

    // Movement
    K[K_MOVE_LEFT]        = CONF_CVAL("key_move_left");
    K[K_MOVE_RIGHT]       = CONF_CVAL("key_move_right");
    K[K_MOVE_UP]          = CONF_CVAL("key_move_up");
    K[K_MOVE_DOWN]        = CONF_CVAL("key_move_down");
    K[K_MOVE_LEFT_UP]     = CONF_CVAL("key_move_left_up");
    K[K_MOVE_RIGHT_UP]    = CONF_CVAL("key_move_right_up");
    K[K_MOVE_LEFT_DOWN]   = CONF_CVAL("key_move_left_down");
    K[K_MOVE_RIGHT_DOWN]  = CONF_CVAL("key_move_right_down");

    // Window control
    K[K_SCROLL_UP]        = CONF_CVAL("key_scroll_up");
    K[K_SCROLL_DOWN]      = CONF_CVAL("key_scroll_down");
    K[K_MENU_LARGE]       = CONF_CVAL("key_menu_large");
    K[K_INVENTORY_LARGE]  = CONF_CVAL("key_inventory_large");
    K[K_CHAT_LARGE]       = CONF_CVAL("key_chat_large");
    K[K_SYSMSG_LARGE]     = CONF_CVAL("key_sysmsg_large");

    // Chat controls
    K[K_CHAT_SEND]        = CONF_CVAL("key_chat_send");

    // Others
    K[K_ZERO]             = CONF_CVAL("key_zero");
    K[K_ONE]              = CONF_CVAL("key_one");
    K[K_TWO]              = CONF_CVAL("key_two");
    K[K_THREE]            = CONF_CVAL("key_three");
    K[K_FOUR]             = CONF_CVAL("key_four");
    K[K_FIVE]             = CONF_CVAL("key_five");
    K[K_SIX]              = CONF_CVAL("key_six");
    K[K_SEVEN]            = CONF_CVAL("key_seven");
    K[K_EIGHT]            = CONF_CVAL("key_eight");
    K[K_NINE]             = CONF_CVAL("key_nine");
}
