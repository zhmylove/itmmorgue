// vim: sw=4 ts=4 et :
#include "config.h"
#include "def_conf.h"
#include "itmmorgue.h"

trie_t *t_conf = NULL; 
int recursion_depth = 6;

enum config_parser_retval parse_option(char *buf, size_t len, size_t *offset,
        char *key, char *value, enum conf_type *type);

void parse_file(char *file);

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
 * opt : value of option to parse
 * rc  : place to save result.
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

    rc->type = CONF_STRING;

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

    rc->type = CONF_INT;

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
 * Used by trie_* to deallocate elements of conf_t type.
 */
void config_deallocator(void *rc) {
    if (NULL == rc) return;
    if (CONF_STRING == ((conf_t*)rc)->type) {
        free(((conf_t*)rc)->sval);
    }
    free(rc);
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
 * Singletone initialization of t_conf with default values
 */
static void config_pre_init() {
    if (t_conf != NULL) {
        panic("config_pre_init() called twice!");
    }

    if ((t_conf = trie_init(config_divisor)) == NULL) {
        panic("Unable to allocate config trie!");
    }

    for (size_t i = 0; i < t_conf_default_size; i++) {
        if (trie_put(t_conf, t_conf_default[i].key,
                    (void*)&(t_conf_default[i].value), sizeof(conf_t),
                    NULL) != 0) {
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

    for (size_t i = 0; i < t_conf_default_size; i++) {
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
    config_pre_init();

    parse_file(file);

#ifdef _DEBUG
    config_dump();
#endif
}

void parse_file(char *file) {
    if (--recursion_depth <= 0) {
        panic("Too big recursion depth");
    }

    struct stat sb;
    int fd, rc;
    char *buf;
    size_t size = 0;

    char *key = (char*)malloc(MAX_OPT_LEN);
    char *value = (char*)malloc(MAX_OPT_LEN);

    if (NULL == key || NULL == value) {
        panic("Cannot allocate memory for config key/value buffers");
    }

    if (file == NULL || ! *file) {
        panic("Invalid file specified for config!");
    }

    if (stat(file, &sb) < 0) {
        panicf("Unable to stat file : %s!", file);
    }

    if (sb.st_size <= 0 || sb.st_size > CONFIG_SIZE_MAX) {
        panicf("Specified config file has invalid size: %ld!", sb.st_size);
    }

    if ((buf = malloc(sb.st_size + 1)) == NULL) {
        panic("Unable to allocate memory for config file!");
    }

    if ((fd = open(file, O_RDONLY, 0)) < 0) {
        panic("Unable to open config file!");
    }

    while (sb.st_size > 0) {
        if ((rc = read(fd, buf + size, sb.st_size)) < 0) {
            panic("Error reading config file!");
        }
        size += rc;
        sb.st_size -= rc;
    }

    buf[size] = '\0';

    size_t buflen = size + 1;
    char *bufptr = buf;
    size_t offset;
    enum conf_type ct;

    while (buflen > 0 && buflen < CONFIG_SIZE_MAX) {
        if (parse_option(bufptr, buflen, &offset, key, value, &ct) !=
                CP_SUCCESS) {
            panic("Error parsing config file!");
        }

        if (*key) {
            conf_t config_value;
            if (CP_SUCCESS != parsers[ct](value, &config_value)) {
                panicf("Error parsing value for %s", key);
            }

            if (trie_put(t_conf, key, (void *)&config_value, sizeof(conf_t),
                       config_deallocator) != 0) {
                panic("Failed to fill t_conf!");
            }
        }

        bufptr += offset;
        buflen -= offset;
    }

    free(value);
    free(key);
    recursion_depth++;
}

/*
 * TODO
 */
enum config_parser_retval parse_option(char *buf, size_t len, size_t *offset,
        char *key, char *value, enum conf_type *type) {
    enum CONFIG_STATE {
        C_WAITKEY,
        C_READ_INC,
        C_WAITKEYCOMMENT,
        C_INKEY,
        C_ESCKEY,
        C_WAITEQ,
        C_WAITVAL,
        C_VALCOMMENT,
        C_INVAL,
        C_ESCVAL,
        C_NULL
    } state = C_WAITKEY;

    size_t off = 0;
    size_t pos = 0;
    conf_t *curr;
    char *include;

    while (off < len) {
        if (pos >= MAX_OPT_LEN - 1) {
            return CP_UNDEF;
        }
        switch (state) {
            case C_WAITKEY:
                if (buf[off] == '.' && buf[off+1] == ' ') {
                    state = C_READ_INC;
                    pos = 0;
                    if ((include = (char*)malloc(MAX_OPT_LEN)) == NULL) {
                        panic("Unable to allocate filename buffer!");
                    }
                    off += 2;
                } else if (buf[off] == '#') {
                    state = C_WAITKEYCOMMENT;
                    off++;
                    continue;
                } else if (isspace(buf[off])) {
                    off++;
                    continue;
                } else if (buf[off] == '\0') {
                    return CP_SUCCESS;
                } else if (buf[off] == '=') {
                    return CP_NO_KEY;
                } else {
                    state = C_INKEY;
                    continue;
                }
                break;
            case C_READ_INC:
                if (buf[off] == '\0') {
                    return CP_UNDEF;
                } else if (buf[off] == '\n') {
                    parse_file(include);
                    key[0] = '\0';
                    *offset = off;
                    return CP_SUCCESS;
                } else {
                    include[pos++] = buf[off++];
                    continue;
                }
                break;
            case C_WAITKEYCOMMENT:
                if (buf[off++] == '\n') {
                    *offset = off;
                    key[0] = '\0';
                    return CP_SUCCESS;
                }
                break;
            case C_INKEY:
                if (buf[off] == '\\') {
                    state = C_ESCKEY;
                    off++;
                    continue;
                } else if (buf[off] == '\n') {
                    return CP_NO_VALUE;
                } else if (buf[off] == '=' || isspace(buf[off])) {
                    state = C_WAITEQ;
                    key[pos] = '\0';
                    pos = 0;
                    if ((curr = (conf_t *)trie_get(t_conf, key)) == NULL) {
                        panicf("Illegal option: %s!", key);
                    }
                    *type = curr->type;
                    continue;
                } else {
                    key[pos++] = buf[off++];
                    continue;
                }
                break;
            case C_ESCKEY:
                key[pos++] = buf[off++];
                state = C_INKEY;
                continue;
                break;
            case C_WAITEQ:
                if (buf[off] == '=') {
                    state = C_WAITVAL;
                    off++;
                    continue;
                } else if (buf[off] == '\n') {
                    return CP_NO_VALUE;
                } else if (isspace(buf[off])) {
                    off++;
                    continue;
                } else {
                    return CP_NO_VALUE;
                }
                break;
            case C_WAITVAL:
                if (buf[off] == '\n') {
                    return CP_NO_VALUE;
                } else if (isspace(buf[off])) {
                    off++;
                    continue;
                } else if (buf[off] == '\0') {
                    return CP_NO_VALUE;
                } else {
                    state = C_INVAL;
                    continue;
                }
                break;
            case C_INVAL:
                if (buf[off] == '#') {
                    state = C_VALCOMMENT;
                    if (value[0] == '\0') {
                        return CP_UNDEF;
                    }
                    value[pos--] = '\0';
                    while (pos > 0 && isspace(value[pos])) {
                        value[pos--] = '\0';
                    }
                    off++;
                    continue;
                } else if (buf[off] == '\\') {
                    state = C_ESCVAL;
                    off++;
                    continue;
                } else if (buf[off] == '\n') {
                    off++;
                    if (value[0] == '\0') {
                        return CP_UNDEF;
                    }
                    value[pos--] = '\0';
                    while (pos > 0 && isspace(value[pos])) {
                        value[pos--] = '\0';
                    }
                    *offset = off;
                    return CP_SUCCESS;
                } else {
                    value[pos++] = buf[off++];
                    continue;
                }
                break;
            case C_ESCVAL:
                value[pos++] = buf[off++];
                state = C_INVAL;
                continue;
                break;
            case C_VALCOMMENT:
                if (buf[off] == '\n') {
                    *offset = off;
                    return CP_SUCCESS;
                } else {
                    off++;
                    continue;
                }
                break;
            case C_NULL:
            default:
                return CP_UNDEF;
        }
    }

    return CP_UNDEF;
}
