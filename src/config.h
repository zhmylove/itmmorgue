// vim: sw=4 ts=4 et :
#ifndef CONFIG_H
#define CONFIG_H

#include <string.h>

// Value of any config parameter, see conf()
typedef union {
    int ival;
    char *sval;
} conf_t;

conf_t conf(char *key);

#endif /* CONFIG_H */
