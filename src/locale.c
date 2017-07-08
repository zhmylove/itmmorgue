// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "config.h"

int size = 0;
trie_t *t_locale = NULL;

char** config_divisor(const char *str);

// Maximum size of locale file
#define LOCALE_SIZE_MAX (16 * 1048576)

/* 
 * Obtains key and value from buf and stores them into corresponding args
 *
 * buf    : buffer to parse
 * len    : maximum buf len
 * offset : place to save parsed length
 * key    : should be allocated
 * value  : should be allocated
 *
 * ret    : parse result
 */
enum config_parser_retval locale_parse(char *buf, size_t len, size_t *offset,
        char *key, char *value) {
    enum LOCALE_STATE {
        L_WAITKEY,
        L_WAITKEYCOMMENT,
        L_INKEY,
        L_ESCKEY,
        L_WAITVAL,
        L_WAITVALCOMMENT,
        L_INVAL,
        L_ESCVAL,
        L_NULL
    } state = L_WAITKEY;

    size_t off = 0;
    size_t pos = 0;

    while (off < len) {
        if (pos >= MAX_OPT_LEN - 1) {
            return CP_UNDEF;
        }
        switch (state) {
            case L_WAITKEY:
                if (buf[off] == '#') {
                    state = L_WAITKEYCOMMENT;
                    off++;
                    continue;
                } else if (buf[off] == '{') {
                    state = L_INKEY;
                    off++;
                    continue;
                } else if (isspace(buf[off])) {
                    off++;
                    continue;
                } else if (buf[off] == '\0') {
                    return CP_SUCCESS;
                } else {
                    return CP_NO_KEY;
                }
                break;
            case L_WAITKEYCOMMENT:
                if (buf[off++] == '\n') {
                    state = L_WAITKEY;
                }
                break;
            case L_INKEY:
                if (buf[off] == '\\') {
                    state = L_ESCKEY;
                    off++;
                    continue;
                } else if (buf[off] == '}') {
                    state = L_WAITVAL;
                    key[pos] = '\0';
                    pos = 0;
                    off++;
                    continue;
                } else {
                    key[pos++] = buf[off++];
                    continue;
                }
                break;
            case L_ESCKEY:
                key[pos++] = buf[off++];
                state = L_INKEY;
                continue;
                break;
            case L_WAITVAL:
                if (buf[off] == '{') {
                    state = L_INVAL;
                    off++;
                    continue;
                } else if (buf[off] == '#') {
                    state = L_WAITVALCOMMENT;
                    off++;
                    continue;
                } else if (buf[off] == '=' || isspace(buf[off])) {
                    off++;
                    continue;
                } else {
                    return CP_NO_VALUE;
                }
                break;
            case L_WAITVALCOMMENT:
                if (buf[off++] == '\n') {
                    state = L_WAITVAL;
                }
                break;
            case L_INVAL:
                if (buf[off] == '\\') {
                    state = L_ESCVAL;
                    off++;
                    continue;
                } else if (buf[off] == '}') {
                    off++;
                    value[pos] = '\0';
                    *offset = off;
                    return CP_SUCCESS;
                } else {
                    value[pos++] = buf[off++];
                    continue;
                }
                break;
            case L_ESCVAL:
                value[pos++] = buf[off++];
                state = L_INVAL;
                continue;
                break;
            case L_NULL:
            default:
                return CP_UNDEF;
        }
    }

    return CP_UNDEF;
}

/* 
 * Initialize locale from file
 */
void locale_init(char *file) {
    struct stat sb;
    int fd, rc;
    char *buf;

    char *key = (char*)malloc(MAX_OPT_LEN);
    char *value = (char*)malloc(MAX_OPT_LEN);

    if (NULL == key || NULL == value) {
        panic("Cannot allocate memory for config key/value buffers");
    }

    if (file == NULL || ! *file) {
        panic("Invalid file specified for locale!");
    }

    if (t_locale != NULL) {
        trie_destroy(t_locale);
    }

    if ((t_locale = trie_init(config_divisor)) == NULL) {
        panic("Unable to create trie for locale!");
    }

    if (stat(file, &sb) < 0) {
        panicf("Unable to stat file: %s!", file);
    }

    if (sb.st_size <= 0 || sb.st_size > LOCALE_SIZE_MAX) {
        panicf("Specified locale file has invalid size: %ld!", sb.st_size);
    }

    if ((buf = malloc(sb.st_size + 1)) == NULL) {
        panic("Unable to allocate memory for l10n file!");
    }

    if ((fd = open(file, O_RDONLY, 0)) < 0) {
        panic("Unable to open l10n file!");
    }

    while (sb.st_size > 0) {
        if ((rc = read(fd, buf + size, sb.st_size)) < 0) {
            panic("Error reading l10n file!");
        }
        size += rc;
        sb.st_size -= rc;
    }

    buf[size] = '\0';

    size_t buflen = size + 1;
    char *bufptr = buf;
    size_t offset;

    while (buflen > 0 && buflen < LOCALE_SIZE_MAX) {
        if (locale_parse(bufptr, buflen, &offset, key, value) != CP_SUCCESS) {
            panic("Error parsing localefile!");
        }

        if (trie_put(t_locale, key, (void *)value, strlen(value) + 1, NULL)
                != 0) {
            panic("Failed to fill t_locale!");
        }

        bufptr += offset;
        buflen -= offset;
    }

    free(value);
    free(key);
}

/* 
 * The only accessor to locale strings from other modules
 */
char *_(char *str) {
    if (t_locale == NULL)  {
        return str;
    }

    char *curr = NULL;

    if ((curr = (char *)trie_get(t_locale, str)) != NULL) {
        return curr;
    }

    return str;
}
