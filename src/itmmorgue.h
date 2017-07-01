// vim: sw=4 ts=4 et :
#ifndef ITMMORGUE_H
#define ITMMORGUE_H

#include <unistd.h>
#if defined(__FreeBSD__) || defined(__linux__)
#include <ncurses.h>
#else
#include <ncurses/ncurses.h>
#endif /* __FreeBSD__ */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <wchar.h>
#include <locale.h>

#include "config.h"

int client(void);

// Panic related definitions
void warn(char *msg);
void panic(char *msg);
#define panicf(fmt, ...)                         \
    do {                                         \
        char buf[BUFSIZ];                        \
        snprintf(buf, BUFSIZ, fmt, __VA_ARGS__); \
        panic(buf);                              \
    } while(0)

#endif /* ITMMORGUE_H */
