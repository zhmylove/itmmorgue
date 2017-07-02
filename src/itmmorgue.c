// vim: sw=4 ts=4 et :
#include "itmmorgue.h"

void warn(char *msg) {
    if (msg) {
        fprintf(stderr, "%s\n", msg);
    }
}

void panic(char *msg) {
    endwin();

    warn(msg);

    exit(EXIT_FAILURE);
}

int strtoi(const char *nptr, char **endptr, int base) {
    long lval = strtol(nptr, endptr, base);
    if (lval < INT_MIN) {
        errno = ERANGE;
        return INT_MIN;
    }
    if (lval > INT_MAX) {
        errno = ERANGE;
        return INT_MAX;
    }
    return (int) lval;
}

size_t anystrnlen(const char *str, size_t maxlen) {
    int len = 0;

    while(maxlen-- && *str) {
        if ((*str++ & 0xC0) != 0x80) {
            len++;
        }
    }

    return len;
}

size_t anystrlen(const char *str) {
    return anystrnlen(str, UINT_MAX);
}

int main(int argc, char *argv[]) {
    // TODO parse argv and run server / client
    // TODO parse config file

    client();

    (void)argc, (void)argv;
    return EXIT_SUCCESS;
}
