// vim: sw=4 ts=4 et :
#include "config.h"
#include "itmmorgue.h"

// Parser for win_ section
conf_t win_conf(char *key) {
    conf_t rc;

    // TODO implement dafault values
    if (key == strstr(key, "stdscr_y")) rc.ival = 0;
    if (key == strstr(key, "stdscr_x")) rc.ival = 0;
    if (key == strstr(key, "stdscr_max_y")) rc.ival = 0;
    if (key == strstr(key, "stdscr_max_x")) rc.ival = 0;
    if (key == strstr(key, "stdscr_state")) rc.ival = 2;

    // TODO implement real parser
    if (key == strstr(key, "area_y")) rc.ival = 3;
    if (key == strstr(key, "area_x")) rc.ival = 2;
    if (key == strstr(key, "area_max_y")) rc.ival = 10;
    if (key == strstr(key, "area_max_x")) rc.ival = 45;
    if (key == strstr(key, "area_state")) rc.ival = 2;

    if (key == strstr(key, "chat_y")) rc.ival = 4;
    if (key == strstr(key, "chat_x")) rc.ival = 49;
    if (key == strstr(key, "chat_max_y")) rc.ival = 7;
    if (key == strstr(key, "chat_max_x")) rc.ival = 7;
    if (key == strstr(key, "chat_state")) rc.ival = 2;

    if (key == strstr(key, "inventory_y")) rc.ival = 6;
    if (key == strstr(key, "inventory_x")) rc.ival = 42;
    if (key == strstr(key, "inventory_max_y")) rc.ival = 12;
    if (key == strstr(key, "inventory_max_x")) rc.ival = 10;
    if (key == strstr(key, "inventory_state")) rc.ival = 2;

    return rc;
}

// Prefixes for config sections/subsections with parser pointers
struct conf_prefix {
    char *prefix;
    conf_t (*parser)(char *);
} pr_global[] = {
    { "win_", win_conf }
};

// Entry point to get all config values
conf_t conf(char *key) {
    conf_t rc;

    for (size_t i = 0;
            i < sizeof(pr_global) / sizeof(struct conf_prefix);
            i++) {
        if (key == strstr(key, pr_global[i].prefix)) {
            rc = win_conf(key + strlen(pr_global[i].prefix));
            break;
        }
    }

    return rc;
}
