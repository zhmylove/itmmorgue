// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "server.h"
#include "stuff.h"

player_t players[MAX_PLAYERS];
size_t players_len = 0;

size_t player_init(enum colors color, char *nickname,
        connection_t *connection) {
    players[players_len].connection = connection;
    players[players_len].color = color;
    strncpy(players[players_len].nickname, nickname, CHAT_NICK_MAXLEN);
    players[players_len].y = 8;
    players[players_len].x = 48;

    return players_len++;
}
