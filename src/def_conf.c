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

    C_INT("win_area_y", 2),
    C_INT("win_area_x", 0),
    C_INT("win_area_max_y", 21),
    C_INT("win_area_max_x", 80),
    C_INT("win_area_state", 2),

    C_INT("win_chat_y", 0),
    C_INT("win_chat_x", 50),
    C_INT("win_chat_max_y", 2),
    C_INT("win_chat_max_x", 30),
    C_INT("win_chat_state", 1),

    C_INT("win_inventory_y", 0),
    C_INT("win_inventory_x", 0),
    C_INT("win_inventory_max_y", 0),
    C_INT("win_inventory_max_x", 0),
    C_INT("win_inventory_state", 0),

    C_INT("win_map_y", 0),
    C_INT("win_map_x", 0),
    C_INT("win_map_max_y", 0),
    C_INT("win_map_max_x", 0),
    C_INT("win_map_state", 0),

    C_INT("win_status_y", 23),
    C_INT("win_status_x", 0),
    C_INT("win_status_max_y", 2),
    C_INT("win_status_max_x", 80),
    C_INT("win_status_state", 2),

    C_INT("win_sysmsg_y", 0),
    C_INT("win_sysmsg_x", 0),
    C_INT("win_sysmsg_max_y", 2),
    C_INT("win_sysmsg_max_x", 50),
    C_INT("win_sysmsg_state", 2),

    C_INT("splash_time", 1000000),
    C_INT("splash_delay", 3000),

    C_STR("locale_file", "")
};
#undef C_STR
#undef C_INT

size_t t_conf_default_size = sizeof(t_conf_default)
    / sizeof(struct t_conf_default);
