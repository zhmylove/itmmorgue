#ifndef ITMMORGUE_H
#define ITMMORGUE_H

#include <unistd.h>
#ifdef __FreeBSD__
#include <ncurses.h>
#else
#include <ncurses/ncurses.h>
#endif /* __FreeBSD__ */
#include <sys/types.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int client(void);
void warn(char *msg);
void panic(char *msg);

#endif /* ITMMORGUE_H */
