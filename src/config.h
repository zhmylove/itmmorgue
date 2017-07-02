// vim: sw=4 ts=4 et :
#ifndef CONFIG_H
#define CONFIG_H

#include <string.h>

enum config_parser_retval {
    CP_SUCCESS,  // Option was parsed successfully
    CP_NO_VALUE, // Given string doesn't contain "="
    CP_NO_KEY,   // "=" is the first character
    CP_TOO_LONG, // Option is longer than it can be
    CP_UNDEF     // An error occurred, but behaviour is undefined
};

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
#define ival value.ival  // ugly definitions for quick access to values
#define sval value.sval
    enum conf_type type; // type of value
} conf_t;

conf_t conf(char *key);

#endif /* CONFIG_H */
