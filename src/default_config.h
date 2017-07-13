#ifdef DEFAULT_CONFIG_H
#error Private DEFAULT_CONFIG_H multiple inclusion!
#endif /* DEFAULT_CONFIG_H */
#define DEFAULT_CONFIG_H

#include "config.h"

/* 
 * Default values for ALL config options!
 * Needs to be updated to implement any new options
 */
#define C_STR(key, val) { key, { { .__sval = val }, CONF_STRING } } 
#define C_INT(key, val) { key, { { .__ival = val }, CONF_INT } } 
#define C_CHR(key, val) { key, { { .__cval = val }, CONF_CHAR } } 
struct t_conf_default {
    char *key;
    conf_t value;
} t_conf_default[] = {
    C_INT("win_stdscr_small_y", 0),
    C_INT("win_stdscr_small_y_ispercent", 0),
    C_INT("win_stdscr_small_x", 0),
    C_INT("win_stdscr_small_x_ispercent", 0),
    C_INT("win_stdscr_small_max_y", 0),
    C_INT("win_stdscr_small_max_y_ispercent", 0),
    C_INT("win_stdscr_small_max_x", 0),
    C_INT("win_stdscr_small_max_x_ispercent", 0),
    C_INT("win_stdscr_large_y", 0),
    C_INT("win_stdscr_large_y_ispercent", 0),
    C_INT("win_stdscr_large_x", 0),
    C_INT("win_stdscr_large_x_ispercent", 0),
    C_INT("win_stdscr_large_max_y", 0),
    C_INT("win_stdscr_large_max_y_ispercent", 0),
    C_INT("win_stdscr_large_max_x", 0),
    C_INT("win_stdscr_large_max_x_ispercent", 0),
    C_INT("win_stdscr_state", 2),

    C_INT("win_area_small_y", 0),
    C_INT("win_area_small_y_ispercent", 0),
    C_INT("win_area_small_x", 0),
    C_INT("win_area_small_x_ispercent", 0),
    C_INT("win_area_small_max_y", 0),
    C_INT("win_area_small_max_y_ispercent", 0),
    C_INT("win_area_small_max_x", 0),
    C_INT("win_area_small_max_x_ispercent", 0),
    C_INT("win_area_large_y", 2),
    C_INT("win_area_large_y_ispercent", 0),
    C_INT("win_area_large_x", 0),
    C_INT("win_area_large_x_ispercent", 0),
    C_INT("win_area_large_max_y", -2),
    C_INT("win_area_large_max_y_ispercent", 0),
    C_INT("win_area_large_max_x", 0),
    C_INT("win_area_large_max_x_ispercent", 0),
    C_INT("win_area_state", 2),

    C_INT("win_chat_small_y", 0),
    C_INT("win_chat_small_y_ispercent", 0),
    C_INT("win_chat_small_x", -30),
    C_INT("win_chat_small_x_ispercent", 1),
    C_INT("win_chat_small_max_y", 2),
    C_INT("win_chat_small_max_y_ispercent", 0),
    C_INT("win_chat_small_max_x", 30),
    C_INT("win_chat_small_max_x_ispercent", 1),
    C_INT("win_chat_large_y", 0),
    C_INT("win_chat_large_y_ispercent", 0),
    C_INT("win_chat_large_x", -30),
    C_INT("win_chat_large_x_ispercent", 1),
    C_INT("win_chat_large_max_y", 0),
    C_INT("win_chat_large_max_y_ispercent", 0),
    C_INT("win_chat_large_max_x", 30),
    C_INT("win_chat_large_max_x_ispercent", 1),
    C_INT("win_chat_state", 1),

    C_INT("win_inventory_small_y", 0),
    C_INT("win_inventory_small_y_ispercent", 0),
    C_INT("win_inventory_small_x", 0),
    C_INT("win_inventory_small_x_ispercent", 0),
    C_INT("win_inventory_small_max_y", 0),
    C_INT("win_inventory_small_max_y_ispercent", 0),
    C_INT("win_inventory_small_max_x", 0),
    C_INT("win_inventory_small_max_x_ispercent", 0),
    C_INT("win_inventory_large_y", 0),
    C_INT("win_inventory_large_y_ispercent", 0),
    C_INT("win_inventory_large_x", 0),
    C_INT("win_inventory_large_x_ispercent", 0),
    C_INT("win_inventory_large_max_y", 0),
    C_INT("win_inventory_large_max_y_ispercent", 0),
    C_INT("win_inventory_large_max_x", 0),
    C_INT("win_inventory_large_max_x_ispercent", 0),
    C_INT("win_inventory_state", 0),

    C_INT("win_map_small_y", 0),
    C_INT("win_map_small_y_ispercent", 0),
    C_INT("win_map_small_x", 0),
    C_INT("win_map_small_x_ispercent", 0),
    C_INT("win_map_small_max_y", 0),
    C_INT("win_map_small_max_y_ispercent", 0),
    C_INT("win_map_small_max_x", 0),
    C_INT("win_map_small_max_x_ispercent", 0),
    C_INT("win_map_large_y", 0),
    C_INT("win_map_large_y_ispercent", 0),
    C_INT("win_map_large_x", 0),
    C_INT("win_map_large_x_ispercent", 0),
    C_INT("win_map_large_max_y", 0),
    C_INT("win_map_large_max_y_ispercent", 0),
    C_INT("win_map_large_max_x", 0),
    C_INT("win_map_large_max_x_ispercent", 0),
    C_INT("win_map_state", 0),

    C_INT("win_status_small_y", 23),
    C_INT("win_status_small_y_ispercent", 0),
    C_INT("win_status_small_x", 0),
    C_INT("win_status_small_x_ispercent", 0),
    C_INT("win_status_small_max_y", 2),
    C_INT("win_status_small_max_y_ispercent", 0),
    C_INT("win_status_small_max_x", 0),
    C_INT("win_status_small_max_x_ispercent", 0),
    C_INT("win_status_large_y", -2),
    C_INT("win_status_large_y_ispercent", 0),
    C_INT("win_status_large_x", 0),
    C_INT("win_status_large_x_ispercent", 0),
    C_INT("win_status_large_max_y", 2),
    C_INT("win_status_large_max_y_ispercent", 0),
    C_INT("win_status_large_max_x", 0),
    C_INT("win_status_large_max_x_ispercent", 0),
    C_INT("win_status_state", 2),

    C_INT("win_sysmsg_small_y", 0),
    C_INT("win_sysmsg_small_y_ispercent", 0),
    C_INT("win_sysmsg_small_x", 0),
    C_INT("win_sysmsg_small_x_ispercent", 0),
    C_INT("win_sysmsg_small_max_y", 2),
    C_INT("win_sysmsg_small_max_y_ispercent", 0),
    C_INT("win_sysmsg_small_max_x", 70),
    C_INT("win_sysmsg_small_max_x_ispercent", 1),
    C_INT("win_sysmsg_large_y", 0),
    C_INT("win_sysmsg_large_y_ispercent", 0),
    C_INT("win_sysmsg_large_x", 0),
    C_INT("win_sysmsg_large_x_ispercent", 0),
    C_INT("win_sysmsg_large_max_y", 100),
    C_INT("win_sysmsg_large_max_y_ispercent", 1),
    C_INT("win_sysmsg_large_max_x", 70),
    C_INT("win_sysmsg_large_max_x_ispercent", 1),
    C_INT("win_sysmsg_state", 1),

    C_INT("splash_time", 1000000),
    C_INT("splash_delay", 3000),

    // TODO: multi-byte keycodes
    C_CHR("key_exit", 'q'),
    C_CHR("key_window_exit", '\t'),
    C_CHR("key_clear_screen", 0x0C),
    C_CHR("key_backspace", 0x7F),

    C_CHR("key_move_left", '4'),
    C_CHR("key_move_right", '6'),
    C_CHR("key_move_up", '8'),
    C_CHR("key_move_down", '2'),
    C_CHR("key_move_left_up", '7'),
    C_CHR("key_move_right_up", '9'),
    C_CHR("key_move_left_down", '1'),
    C_CHR("key_move_right_down", '3'),

    C_CHR("key_menu_large", 'm'),
    C_CHR("key_inventory_large", 'i'),
    C_CHR("key_chat_large", 'c'),
    C_CHR("key_sysmsg_large", 'P'),

    C_CHR("key_chat_send", '\n'),

    C_CHR("key_zero", '0'),
    C_CHR("key_one", '1'),
    C_CHR("key_two", '2'),
    C_CHR("key_three", '3'),
    C_CHR("key_four", '4'),
    C_CHR("key_five", '5'),
    C_CHR("key_six", '6'),
    C_CHR("key_seven", '7'),
    C_CHR("key_eight", '8'),
    C_CHR("key_nine", '9'),

    C_STR("player_nickname", ""),
    C_STR("file_locale", ""),
    C_STR("file_server_log", "itmmorgue.log")
};
#undef C_STR
#undef C_INT

size_t t_conf_default_size = sizeof(t_conf_default)
    / sizeof(struct t_conf_default);
