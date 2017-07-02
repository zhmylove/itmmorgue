// vim: sw=4 ts=4 et :
#include "config.h"
#include "itmmorgue.h"

enum config_parser_retval {
    CP_SUCCESS,  // Option was parsed successfully
    CP_NO_VALUE, // Given string doesn't contain "="
    CP_NO_KEY,   // "=" is the first character
    CP_TOO_LONG, // Option is longer than it can be
    CP_UNDEF     // An error occurred, but behaviour is undefined
};

static enum config_parser_retval
parse_option(const char *opt, conf_t *rc, enum conf_type ct, char *key);

// Parsers for concrete value types, called by parse_option()
static enum config_parser_retval
parse_option_string(const char *opt, conf_t *rc);

static enum config_parser_retval
parse_option_int(const char *opt, conf_t *rc);

static enum config_parser_retval (*parsers[])(const char *opt, conf_t *rc) = {
    parse_option_string,
    parse_option_int
};

// TODO: deprecated, delete after config system will be implemented
// Parser for win_ section
static conf_t win_conf(char *key) {
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

// TODO: deprecated, delete after config system will be implemented
static conf_t splash_conf(char *key) {
    conf_t rc;

    if (key == strstr(key, "delay")) rc.ival = 30000;
    if (key == strstr(key, "time")) rc.ival = 1000000;

    return rc;
}

// TODO: deprecated, delete after config system will be implemented
// Prefixes for config sections/subsections with parser pointers
struct conf_prefix {
    char *prefix;
    conf_t (*parser)(char *);
} pr_global[] = {
    { "win_", win_conf },
    { "splash_", splash_conf }
};

// TODO: deprecated, delete after config system will be implemented
// Entry point to get all config values
conf_t conf(char *key) {
    conf_t rc;
    parse_option(NULL, NULL, CONF_INT, NULL);

    rc.type = CONF_INT;
    for (size_t i = 0;
            i < sizeof(pr_global) / sizeof(struct conf_prefix);
            i++) {
        if (key == strstr(key, pr_global[i].prefix)) {
            rc = pr_global[i].parser(key + strlen(pr_global[i].prefix));
            break;
        }
    }

    return rc;
}

/*
 * Maximum length of option, includes length both of *key* and *value* in
 * "key=value" pair.
 */
#define MAX_OPT_LEN 256

/*
 * Splits opt by "=", saves the first part to key and the second into rc
 *
 * opt : string, which looks like "key=value"
 * rc  : place to save result
 * ct  : type of value, defines, how it will be parsed
 * key : place to save 'key' or NULL if key isn't needed
 *
 * ret : one of config_parser_retval enumeration values
 */
static enum config_parser_retval
parse_option(const char *opt, conf_t *rc, enum conf_type ct, char *key) {
    if (NULL == opt) return CP_SUCCESS;
    if (NULL == opt) {
        panic("NULL passed as a config option");
    }

    // if opt is '=value'
    if ('=' == opt[0]) {
        return CP_NO_KEY;
    }

    size_t opt_len = strnlen(opt, MAX_OPT_LEN);
    if (opt_len == MAX_OPT_LEN) {
        // Option is too long
        return CP_TOO_LONG;
    }

    // Let's find value of option
    const char *eq_pos = opt;
    while (*eq_pos++ != '=' && *eq_pos);

    // if opt is 'key'
    if ('=' != *(eq_pos-1)) {
        return CP_NO_VALUE;
    }

    // if opt is 'key='
    if (*(eq_pos) == '\0') {
        /* TODO: what are we gonna do with smth like 'key='? */
        // TODO: save key
        return CP_UNDEF;
    }

    // clangs tells it's always false.
    // It will be truth until somebody casts int to enum.
    //if (ct > sizeof(parsers) / sizeof(parsers[0])) {
    //    panic("Trying to call undefined parser!");
    //}

    // Save key
    // TODO: test it all!
    if (NULL != key) {
        strncpy(key, opt, eq_pos - key - 1);
        *(key + (eq_pos - opt - 1)) = '\0';
    }

    rc->type = ct;
    // Call suitable parser
    return parsers[ct](eq_pos, rc);
}

/*
 * opt : value of option to parse
 * rc  : place to save result. .type won't be filled
 *
 * ret : one of config_parser_retval enumeration values
 */
static enum config_parser_retval
parse_option_string(const char *opt, conf_t *rc) {
    rc->sval = (char*) malloc(strnlen(opt, MAX_OPT_LEN));
    if (NULL == rc->sval) {
        panicf("Allocation failed in file %s, line %d", __FILE__, __LINE__);
    }

    strncpy(rc->sval, opt, MAX_OPT_LEN);
    return CP_SUCCESS;
}

/*
 * opt : value of option to parse
 * rc  : place to save result
 *
 * ret : one of config_parser_retval enumeration values
 */
static enum config_parser_retval
parse_option_int(const char *opt, conf_t *rc) {
    char *endptr = NULL;
    int value = strtoi(opt, &endptr, 0 /* Not only decimal */);

    if (ERANGE == errno) {
        return CP_TOO_LONG;
    }
    rc->ival = value;

    return CP_SUCCESS;
}

#undef MAX_OPT_LEN
