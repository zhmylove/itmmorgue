// vim: sw=4 ts=4 et :
#include "config.h"

// Parser for win_ section
conf_t win_conf(char *key) {
    conf_t rc;

    rc.ival = 0;

    (void)key;
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

    for (size_t i = 0; i < sizeof(pr_global); i++) {
        if (strstr(key, pr_global[i].prefix) == 0) {
            rc = win_conf(key + strlen(pr_global[i].prefix));
            break;
        }
    }

    return rc;
}
