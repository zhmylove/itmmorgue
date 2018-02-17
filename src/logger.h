#ifndef LOGGER_H
#define LOGGER_H

int log_fd;

void logger_init();
void logger(char *str);

#define loggerf(fmt, ...)                           \
    do {                                            \
        char ___buf[BUFSIZ];                        \
        snprintf(___buf, BUFSIZ, fmt, __VA_ARGS__); \
        logger(___buf);                             \
    } while(0)

#endif /* LOGGER_H */

