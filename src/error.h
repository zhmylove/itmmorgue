#ifndef PANIC_H
#define PANIC_H

#include <stdlib.h>
#include <stdio.h>

#if defined(__FreeBSD__) || defined(__linux__)
#include <ncurses.h>
#include <sys/uio.h>
#else
#include <ncurses/ncurses.h>
#endif /* __FreeBSD__ || __linux__ */

void warn(const char * const msg);
void panic(const char * const msg);

#define warnf(fmt, ...)                             \
    do {                                            \
        char ___buf[BUFSIZ];                        \
        snprintf(___buf, BUFSIZ, fmt, __VA_ARGS__); \
        warn(___buf);                               \
    } while(0)

#define panicf(fmt, ...)                            \
    do {                                            \
        char ___buf[BUFSIZ];                        \
        snprintf(___buf, BUFSIZ, fmt, __VA_ARGS__); \
        panic(___buf);                              \
    } while(0)

#endif /* PANIC_H */
