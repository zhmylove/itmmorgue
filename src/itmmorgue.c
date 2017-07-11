// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "server.h"
#include "client.h"

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

#ifdef __sun
size_t strnlen(const char *str, size_t maxlen) {
    size_t rc = 0;
    
    while (maxlen-- && *str++) {
        rc++;
    }

    return rc;
}
#endif /* __sun */

int main(int argc, char *argv[]) {
    // TODO parse argv and run server / client

    int server_only = 0;

    server_started = 0;
    server_connected = 0;

    config_init("itmmorgue.conf");

    while (*++argv) {
        if (strcmp(*argv, "--server-only") == 0) {
            server_only = 1;
        } else if (strcmp(*argv, "-s") == 0) {
            server_only = 1;
        }
    }

    if (server_only == 0) {
        client();
    } else {
        fprintf(stderr, "Starting server in headless mode...\n");
        server();
    }

    (void) argc;
    return EXIT_SUCCESS;
}
