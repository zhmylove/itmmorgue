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

void c_set_players(players_mbuf_t *mbuf) {
    if (!mbuf || mbuf->players_len >= MAX_PLAYERS) return;

    for (players_len = 0; players_len < mbuf->players_len; players_len++) {
        players[players_len] = mbuf->players[players_len];
    }
}

void s_players_send(player_t *player) {
    players_mbuf_t* players_mbuf;

    if ((players_mbuf = (players_mbuf_t *)malloc(sizeof(players_mbuf_t))) ==
            NULL) {
        panic("Error creating players_mbuf for send!");
    }
    memcpy(players_mbuf->players, players, sizeof(players));
    players_mbuf->players_len = players_len;

    mbuf_t s2c_mbuf;
    s2c_mbuf.payload = (void *)players_mbuf;
    s2c_mbuf.msg.type = MSG_PUT_PLAYERS;
    s2c_mbuf.msg.size = sizeof(players_mbuf_t);

    logger("[S] Sending players");
    mqueue_put(player->connection->mqueueptr, s2c_mbuf);
}

