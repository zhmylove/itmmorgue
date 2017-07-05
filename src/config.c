// vim: sw=4 ts=4 et :
#include "config.h"
#include "itmmorgue.h"

trie_t *t_conf = NULL; 

static enum config_parser_retval parse_option(const char *opt, conf_t *rc,
        enum conf_type ct, char *key);

// Parsers for specific value types, called by parse_option()
static enum config_parser_retval parse_option_string(const char *opt,
        conf_t *rc);

static enum config_parser_retval parse_option_int(const char *opt, conf_t *rc);

static enum config_parser_retval (*parsers[])(const char *opt, conf_t *rc) = {
    parse_option_string,
    parse_option_int
};

/* 
 * The only accessor to config values from other modules
 */
conf_t conf(char *key) {
    conf_t *rc;

    if ((rc = (conf_t *)trie_get(t_conf, key)) != NULL) {
        return *rc;
    } else {
        panicf("Invalid conf(%s) requested!", key);
    }

    return *rc;
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
    size_t len = strnlen(opt, MAX_OPT_LEN);
    rc->sval = (char*) malloc(len);

    if (NULL == rc->sval) {
        panicf("Allocation failed in file %s, line %d", __FILE__, __LINE__);
    }

    strncpy(rc->sval, opt, len);

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

/* 
 * Remove spaces from the beginning and the end of the string
 */
void remove_spaces(char **str) {
    size_t ptr = strlen(*str) - 1;

    while (ptr > 0 && isspace((*str)[ptr])) {
        (*str)[ptr--] = '\0';
    }

    while (**str && isspace(**str)) {
        (*str)++;
    }
}

/* 
 * Used by trie_* to split keys
 * In config case perl equivalent is: split /_/, str
 */
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
    C_INT("win_stdscr_y", 0),
    C_INT("win_stdscr_x", 0),
    C_INT("win_stdscr_max_y", 0),
    C_INT("win_stdscr_max_x", 0),
    C_INT("win_stdscr_state", 2),

    C_INT("win_area_y", 3),
    C_INT("win_area_x", 2),
    C_INT("win_area_max_y", 10),
    C_INT("win_area_max_x", 45),
    C_INT("win_area_state", 2),

    C_INT("win_chat_y", 4),
    C_INT("win_chat_x", 49),
    C_INT("win_chat_max_y", 7),
    C_INT("win_chat_max_x", 7),
    C_INT("win_chat_state", 2),

    C_INT("win_inventory_y", 6),
    C_INT("win_inventory_x", 42),
    C_INT("win_inventory_max_y", 12),
    C_INT("win_inventory_max_x", 10),
    C_INT("win_inventory_state", 2),

    C_INT("splash_time", 1000000),
    C_INT("splash_delay", 30000),

    C_STR("locale_file", "")
};
#undef C_STR
#undef C_INT

/* 
 * Singletone initialization of t_conf with default values
 */
static void config_pre_init() {
    if (t_conf != NULL) {
        panic("config_pre_init() called twice!");
    }

    if ((t_conf = trie_init(config_divisor)) == NULL) {
        panic("Unable to allocate config trie!");
    }

    for (size_t i = 0;
            i < sizeof(t_conf_default) / sizeof(struct t_conf_default);
            i++) {
        if (trie_put(t_conf, t_conf_default[i].key,
                    (void*)&(t_conf_default[i].value), sizeof(conf_t)) != 0) {
            panicf("Failed to initialize t_conf[%s]!", t_conf_default[i].key);
        }
    }
}

/* 
 * Dump config to STDERR for _DEBUG purposes.
 */
void config_dump() {
    conf_t *curr;

    fprintf(stderr, " === Config dump === \n");

    for (size_t i = 0;
            i < sizeof(t_conf_default) / sizeof(struct t_conf_default);
            i++) {
        char *key = t_conf_default[i].key;

        if ((curr = (conf_t *)trie_get(t_conf, key)) != NULL) {
            switch (curr->type) {
                case CONF_STRING:
                    fprintf(stderr, "conf[%s] = %s\n", key, curr->sval);
                    break;
                case CONF_INT:
                    fprintf(stderr, "conf[%s] = %d\n", key, curr->ival);
                    break;
                default:
                    panic("config_dump()::switch() needs fix!");
            }
        } else {
            panic("Memory corruption!");
        }
    }
}

/* 
 * Initialize configuration from file
 */
void config_init(char *file) {
    int fd, rc;
    char *buf;
    struct stat sb;
    char *keybuf = malloc(MAX_OPT_LEN + 1);
    char *key = keybuf;

    if (file == NULL || ! *file) {
        panic("Invalid file specified for config!");
    }

    config_pre_init();

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

    int i = 0, end = 0;

    if ((rc = read(fd, buf, sb.st_size)) < 0) {
        panic("Error reading config file!");
    }

    buf[rc] = '\0';

    do {
        key = keybuf; // due to remove_spaces() may change it

        while (*buf && buf[i] != '\n') i++;

        if (! *buf) {
            end = 1;
        }
        if (i == 0) {
            buf++;
            continue;
        }
        buf[i] = '\0';

        enum conf_type ct;
        int parse_result;
        conf_t value;

        // This double-parsing code makes my eyes full of tears
        char *eq = strstr(buf, "=");
        if (eq == NULL) {
            goto EILLOPT;
        }
        memcpy(key, buf, eq - buf);
        key[eq - buf] = '\0';

        remove_spaces(&key);

        conf_t *curr;
        if ((curr = (conf_t *)trie_get(t_conf, key)) != NULL) {
            ct = curr->type;

            if ((parse_result = parse_option(buf, &value, ct, key)) !=
                    CP_SUCCESS) {
                panicf("Error parsing config: %d", parse_result);
            }

            remove_spaces(&key);

            if (trie_put(t_conf, key, (void *)&value, sizeof(conf_t)) != 0) {
                panic("Failed to fill t_conf!");
            }
        } else {
EILLOPT:
            warnf("Unknown option specified: %s", key);
        }

        buf += i + 1;
        i = 0;
    } while (! end);

#ifdef _DEBUG
    config_dump();
#endif /* _DEBUG */
    return;
}
