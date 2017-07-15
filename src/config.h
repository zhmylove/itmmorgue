// vim: sw=4 ts=4 et :
#ifndef CONFIG_H
#define CONFIG_H

#include <string.h>

/*
 * Maximum length of option, includes length both of *key* and *value* in
 * "key=value" pair.
 */
#define MAX_OPT_LEN 4096
#define CONFIG_SIZE_MAX (16 * 1048576)

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
    CONF_INT    = 1,
    CONF_CHAR   = 2  // TODO: should it be a multi-byte char?
};

// Value of any config parameter, see conf()
typedef struct conf {
    union {
        int __ival;
        char *__sval;
        char __cval;
    } value;               // config value
#define ival value.__ival  // ugly definitions for quick access to values
#define sval value.__sval
#define cval value.__cval
    enum conf_type type;   // type of value
} conf_t;

// Definitions for quick config value access
#define CONF_IVAL(x) conf(x).value.__ival
#define CONF_SVAL(x) conf(x).value.__sval
#define CONF_CVAL(x) conf(x).value.__cval

conf_t conf(char *key);
void config_init(char *file);

#endif /* CONFIG_H */
