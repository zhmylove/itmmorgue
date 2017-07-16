// vim: sw=4 ts=4 et :
#ifndef PLAYER_H
#define PLAYER_H

#include "itmmorgue.h"

#define MAX_PLAYERS 3

typedef struct player {
    enum colors color;          // server-specified attributes
    uint16_t y;                 // absolute Y
    uint16_t x;                 // absolute X
    char nickname[PLAYER_NAME_MAXLEN];
    connection_t *connection;
    /* inventory_t */
    /* common creatures stats_t */
    /* player specific stats ?? */
    /* spell_list_t */
    /* skills_t */
} player_t;

size_t player_init(enum colors color, char *nickname,
        connection_t *connection);

extern player_t players[];
extern size_t players_len;

#endif /* PLAYER_H */
