// vim: sw=4 ts=4 et :
#ifndef PLAYER_H
#define PLAYER_H

#include "itmmorgue.h"
#include "entity.h"

#define MAX_PLAYERS 5

typedef struct player_move {
    enum keyboard direction;
    size_t player_id;
} player_move_t;

size_t player_init(enum colors color, char *nickname,
        connection_t *connection);
void c_send_move(enum keyboard last_key);
void s_send_players_full(entity_t* player);

extern entity_t* players2[];
extern size_t players2_id[];
extern size_t players_len;
extern size_t player_self;
extern size_t players_total;

#endif /* PLAYER_H */
