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
    C_INT("win_sysmsg_small_max_x", 50),
    C_INT("win_sysmsg_small_max_x_ispercent", 0),
    C_INT("win_sysmsg_large_y", 0),
    C_INT("win_sysmsg_large_y_ispercent", 0),
    C_INT("win_sysmsg_large_x", 0),
    C_INT("win_sysmsg_large_x_ispercent", 0),
    C_INT("win_sysmsg_large_max_y", 2),
    C_INT("win_sysmsg_large_max_y_ispercent", 0),
    C_INT("win_sysmsg_large_max_x", 70),
    C_INT("win_sysmsg_large_max_x_ispercent", 1),
    C_INT("win_sysmsg_state", 2),

    C_INT("splash_time", 1000000),
    C_INT("splash_delay", 3000),

    C_STR("locale_file", "")
};
#undef C_STR
#undef C_INT

size_t t_conf_default_size = sizeof(t_conf_default)
    / sizeof(struct t_conf_default);