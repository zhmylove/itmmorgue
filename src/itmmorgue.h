// vim: sw=4 ts=4 et :
#ifndef ITMMORGUE_H
#define ITMMORGUE_H

#include <unistd.h>
#ifdef __FreeBSD__
#include <ncurses.h>
#else
#include <ncurses/ncurses.h>
#endif /* __FreeBSD__ */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <wchar.h>
#include <locale.h>

int client(void);
void warn(char *msg);
void panic(char *msg);

#endif /* ITMMORGUE_H */
