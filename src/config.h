// vim: sw=4 ts=4 et :
#ifndef CONFIG_H
#define CONFIG_H

#include <string.h>

// Type of configuration entity
enum conf_type {
    CONF_STRING = 0,
    CONF_INT    = 1
};

// Value of any config parameter, see conf()
typedef struct conf {
    union {
        int ival;
        char *sval;
    } value;             // config value
#define ival value.ival  // ugly definitons for quick access to values
#define sval value.sval
    enum conf_type type; // type of value
} conf_t;

conf_t conf(char *key);

#endif /* CONFIG_H */
