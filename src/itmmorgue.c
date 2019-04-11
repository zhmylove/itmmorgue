// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "server.h"
#include "client.h"

int server_only = 0;

void warn(char *msg) {
    if (msg) {
        fprintf(stderr, "%s\n", msg);
    }

    if (_LOG_PANIC) {
        logger(msg);
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
    size_t rc = 0;

    while (maxlen-- && *str++) {
        rc++;
    }

    return rc;
}
#endif /* __sun */

int synchronized_readall(pthread_mutex_t *mutex, int fd, void *buf,
        size_t size) {
    if (NULL == mutex) {
        panic("synchronized_readall: NULL mutex provided");
    }

    pthread_mutex_lock(mutex);
    int retval = readall(fd, buf, size);
    pthread_mutex_unlock(mutex);
    return retval;
}

int readall(int fd, void *buf, size_t size) {
    int rc = -1;
    size_t got = 0;

    if (size == 0) {
        return 0;
    }
    while (got < size) {
        if ((rc = read(fd, (char *)buf + got, size - got)) > 0) {
            got += rc;
        } else {
            break;
        }
    }

    if (got == size) {
        return size;
    }

    return -1;
}

void log_init() {
    char *log_file = CONF_SVAL("file_server_log");
    if (! *log_file) {
        return;
    }

    if ((log_fd = open(log_file, O_WRONLY | O_APPEND | O_CREAT, 0666)) < 0) {
        panicf("Unable to open %s!", log_file);
    }

    logger(" ======= GAME STARTED ======= ");
}

void logger(char *str) {
    if (log_fd < 0) {
        return;
    }

    char buf[8192];
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) {
        panic("Unable to get system time!");
    }

    int len = snprintf(buf, sizeof(buf), "%lu: %s\n", tv.tv_sec, str);
    if (write(log_fd, buf, len) < 0) {
        panic("Unable to write log output!");
    }

    if (server_only == 1) {
        if (write(2, buf, len) < 0) {
            panic("Unable to write log to stderr!");
        }
    }
}

unsigned long long systime() {
    struct timeval tv;

    if (gettimeofday(&tv, NULL) < 0) {
        panic("Unable to get system time!");
    }

    return tv.tv_sec;
}

unsigned long long sysutime() {
    struct timeval tv;

    if (gettimeofday(&tv, NULL) < 0) {
        panic("Unable to get system time!");
    }

    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char *argv[]) {
    // TODO parse argv and run server / client

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

    log_fd = -1;
    log_init();

    if (server_only == 0) {
        client();
    } else {
        fprintf(stderr, "Starting server in headless mode...\n");
        server();
    }

    (void) argc;
    return EXIT_SUCCESS;
}
