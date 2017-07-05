// vim: sw=4 ts=4 et :
#include "config.h"
#include "itmmorgue.h"

static enum config_parser_retval parse_option(const char *opt, conf_t *rc,
        enum conf_type ct, char *key);

// Parsers for concrete value types, called by parse_option()
static enum config_parser_retval parse_option_string(const char *opt,
        conf_t *rc);

static enum config_parser_retval parse_option_int(const char *opt, conf_t *rc);

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
 * Splits opt by "=", saves the first part to key and the second into rc
 *
 * opt : string, which looks like "key=value"
 * rc  : place to save result
 * ct  : type of value, defines, how it will be parsed
 * key : place to save 'key' or NULL if key isn't needed
 *
 * ret : one of config_parser_retval enumeration values
 */
static enum config_parser_retval parse_option(const char *opt, conf_t *rc,
        enum conf_type ct, char *key) {
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

    // Save key
    // TODO: test it all!
    if (NULL != key) {
        strncpy(key, opt, eq_pos - opt - 1);
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
static enum config_parser_retval parse_option_string(const char *opt,
        conf_t *rc) {
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
static enum config_parser_retval parse_option_int(const char *opt,
        conf_t *rc) {
    char *endptr = NULL;
    int value = strtoi(opt, &endptr, 0 /* Not only decimal */);

    if (ERANGE == errno) {
        return CP_TOO_LONG;
    }
    rc->ival = value;

    return CP_SUCCESS;
}

char** config_divisor(const char *str) {
    int i = 0, j = 0;
    char **rc = malloc(sizeof(char*));

    while (*str) {
        while (str[i] && str[i] != '_') i++;

        if (i == 0) {
            str++;
            continue;
        }

        // we have no need in memory deallocation ;-)
        if ((rc = (char **)realloc(rc, sizeof(char *) * (j + 2))) == NULL) {
            panic("Unable to realloc in config_divisor!");
        }

        if ((rc[j] = malloc(i + 1)) == NULL) {
            panic("Unable to allocate memory in config_divisor!");
        }

        memcpy(rc[j], str, i);
        rc[j++][i] = '\0';

        str += i;
        i = 0;
    }

    rc[j] = NULL;

    return rc;
}

trie_t *t_conf; 

void config_init(char *file) {
    int fd, rc;
    char *buf;
    struct stat sb;
    char *key = malloc(MAX_OPT_LEN);

    if (file == NULL || ! *file) {
        panic("Invalid file specified for config!");
    }

    if (stat(file, &sb) < 0) {
        panicf("Unable to get info : %s!", file);
    }

#define CONFIG_SIZE_MAX (16 * 1048576)
    if (sb.st_size <= 0 || sb.st_size > CONFIG_SIZE_MAX) {
        panicf("Specified config file has invalid size: %ld!", sb.st_size);
    }

    if ((buf = malloc(sb.st_size + 1)) == NULL) {
        panic("Unable to allocate configuration buffer!");
    }

    if ((fd = open(file, O_RDONLY, 0)) < 0) {
        panic("Unable to open config file!");
    }

    if ((t_conf = trie_init(config_divisor)) == NULL) {
        panic("Unable to allocate config trie!");
    }

    int i = 0, end = 0;

    if ((rc = read(fd, buf, sb.st_size)) < 0) {
        panic("Error reading config file!");
    }

    buf[rc] = '\0';

    do {
        while (*buf && buf[i] != '\n') i++;

        if (! *buf) {
            end = 1;
        }
        if (i == 0) {
            buf++;
            continue;
        }
        buf[i] = '\0';

        // TODO check option name and fill conf_type
        enum conf_type ct = CONF_INT;

        int parse_result;
        conf_t value;

        if ((parse_result = parse_option(buf, &value, ct, key)) !=
                CP_SUCCESS) {
            panicf("Error parsing config: %d", parse_result);
        }

        if (trie_put(t_conf, key, (void *)&value, sizeof(conf_t)) != 0) {
            panic("Failed to fill t_conf!");
        }

        buf += i + 1;
        i = 0;
    } while (! end);

    return;
}
