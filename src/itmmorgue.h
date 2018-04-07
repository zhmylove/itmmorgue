// vim: sw=4 ts=4 et :
#ifndef ITMMORGUE_H
#define ITMMORGUE_H

#include <unistd.h>
#if defined(__FreeBSD__) || defined(__linux__)
#include <ncurses.h>
#include <sys/uio.h>
#else
#include <ncurses/ncurses.h>
#endif /* __FreeBSD__ || __linux__ */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>
#include <limits.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>

// Dark and light colors
enum colors {
    D_WHITE, D_YELLOW, D_RED, D_GREEN, D_BLUE, D_BLACK, D_CYAN, D_MAGENTA,
    L_WHITE, L_YELLOW, L_RED, L_GREEN, L_BLUE, L_BLACK, L_CYAN, L_MAGENTA
};

#define PLAYER_NAME_MAXLEN 32

#include "config.h"
#include "keyboard.h"
#include "event.h"
#include "protocol.h"
#include "connection.h"
#include "player.h"
#include "client.h"
#include "server.h"
#include "trie/trie.h"
#include "stuff.h"
#include "tiles.h"
#include "levels.h"
#include "config.h"
#include "error.h"
#include "ioutils.h"
#include "strutils.h"
#include "logger.h"

int server_only;
void player_move(player_move_t *move);


#endif /* ITMMORGUE_H */
