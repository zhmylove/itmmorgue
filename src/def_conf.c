#include "def_conf.h"

/* 
 * Default values for ALL config options!
 * Needs to be updated to implement any new options
 */
#define C_STR(key, val) { key, { { .__sval = val }, CONF_STRING } } 
#define C_INT(key, val) { key, { { .__ival = val }, CONF_INT } } 
struct t_conf_default t_conf_default[] = {
    C_INT("win_stdscr_y", 0),
    C_INT("win_stdscr_x", 0),
    C_INT("win_stdscr_max_y", 0),
    C_INT("win_stdscr_max_x", 0),
    C_INT("win_stdscr_state", 2),

    C_INT("win_area_y", 3),
    C_INT("win_area_x", 2),
    C_INT("win_area_max_y", 10),
    C_INT("win_area_max_x", 45),
    C_INT("win_area_state", 2),

    C_INT("win_chat_y", 4),
    C_INT("win_chat_x", 49),
    C_INT("win_chat_max_y", 7),
    C_INT("win_chat_max_x", 7),
    C_INT("win_chat_state", 2),

    C_INT("win_inventory_y", 6),
    C_INT("win_inventory_x", 42),
    C_INT("win_inventory_max_y", 12),
    C_INT("win_inventory_max_x", 10),
    C_INT("win_inventory_state", 2),

    C_INT("splash_time", 1000000),
    C_INT("splash_delay", 3000),

    C_STR("locale_file", "")
};
#undef C_STR
#undef C_INT

size_t t_conf_default_size = sizeof(t_conf_default)
    / sizeof(struct t_conf_default);
