#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include "strutils.h"

int strtoi(const char *nptr, char **endptr, int base) {
    long lval = strtol(nptr, endptr, base);

    if (lval < INT_MIN) {
        errno = ERANGE;
        return INT_MIN;
    } else if (lval > INT_MAX) {
        errno = ERANGE;
        return INT_MAX;
    }

    return (int) lval;
}

size_t anystrunplen(char *str, size_t maxlen, char ** endp) {
    int len = 0;

    char *last_sym = str;

    while (maxlen-- && *str) {
        if ((*str++ & 0xC0) != 0x80) {
            len++;
            last_sym = str - 1;
        } else {
            maxlen++;
        }
    }

    if (++maxlen == 0 && endp != NULL && *endp != NULL && *str) {
        *endp = last_sym;
    }

    return len;
}

size_t anystrnplen(char *str, size_t maxlen, char ** endp) {
    int len = 0;

    while (maxlen-- && *str) {
        if ((*str++ & 0xC0) != 0x80) {
            len++;
        }
    }

    if (++maxlen == 0 && endp != NULL && *endp != NULL && *str) {
        *endp = str;
    }

    return len;
}

size_t anystrnlen(char *str, size_t maxlen) {
    return anystrnplen(str, maxlen, NULL);
}

size_t anystrlen(char *str) {
    return anystrnlen(str, UINT_MAX);
}

#ifdef __sun
size_t strnlen(const char *str, size_t maxlen) {
    size_t rc = 0; for (;maxlen-- && *str++; rc++);
    return rc;
}
#endif /* __sun */
