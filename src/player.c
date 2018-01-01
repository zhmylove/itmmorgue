// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "server.h"
#include "stuff.h"

player_t players[MAX_PLAYERS];
size_t players_len = 0;
size_t player_self = 0;
size_t players_total = 0;

size_t player_init(enum colors color, char *nickname,
        connection_t *connection) {
    players[players_len].connection = connection;
    players[players_len].color = color;
    strncpy(players[players_len].nickname, nickname, CHAT_NICK_MAXLEN);
    players[players_len].ready = 0;

    if (start != 3) {
        /* 
         * We don't need to set y & x on dead players.
         * They'll be restored during NICKNAME reception.
         */
        players[players_len].y = 8;
        players[players_len].x = 48;
    }

    return players_len++;
}

void c_receive_players_full(players_full_mbuf_t *mbuf) {
    if (!mbuf || mbuf->players_len >= MAX_PLAYERS) return;

    player_self = mbuf->self;

    /* Maybe we shouldn't send connection, etc. */
    for (players_len = 0; players_len < mbuf->players_len; players_len++) {
        players[players_len] = mbuf->players[players_len];
    }
}

void c_receive_players(players_mbuf_t *mbuf) {
    if (!mbuf || mbuf->players_len >= MAX_PLAYERS) return;

    player_self = mbuf->self;

    for (players_len = 0; players_len < mbuf->players_len; players_len++) {
        players[players_len].color = mbuf->players[players_len].color ;
        players[players_len].y     = mbuf->players[players_len].y     ;
        players[players_len].x     = mbuf->players[players_len].x     ;
    }
}

void s_send_players_full(player_t *player) {
    players_full_mbuf_t* players_mbuf;

    if (! player->connected) {
        return;
    }

    if ((players_mbuf = (players_full_mbuf_t *)malloc(
                    sizeof(players_full_mbuf_t)
                    )) == NULL) {
        panic("Error creating players_full_mbuf for send!");
    }
    for (size_t i = 0; i < players_len; i++) {
        if (player->connection == players[i].connection) {
            players_mbuf->self = i;
        }
        players_mbuf->players[i] = players[i];
    }
    players_mbuf->players_len = players_len;

    mbuf_t s2c_mbuf;
    s2c_mbuf.payload = (void *)players_mbuf;
    s2c_mbuf.msg.type = MSG_PUT_PLAYERS_FULL;
    s2c_mbuf.msg.size = sizeof(players_full_mbuf_t);

    logger("[S] Sending players full");
    mqueue_put(player->connection->mqueueptr, s2c_mbuf);
}

void s_send_players(player_t *player) {
    players_mbuf_t* players_mbuf;

    if (! player->connected) {
        return;
    }

    if ((players_mbuf = (players_mbuf_t *)malloc(sizeof(players_mbuf_t))) ==
            NULL) {
        panic("Error creating players_mbuf for send!");
    }
    for (size_t i = 0; i < players_len; i++) {
        if (player->connection == players[i].connection) {
            players_mbuf->self = i;
        }
        players_mbuf->players[i].color = players[i].color;
        players_mbuf->players[i].y     = players[i].y;
        players_mbuf->players[i].x     = players[i].x;
    }
    players_mbuf->players_len = players_len;

    mbuf_t s2c_mbuf;
    s2c_mbuf.payload = (void *)players_mbuf;
    s2c_mbuf.msg.type = MSG_PUT_PLAYERS;
    s2c_mbuf.msg.size = sizeof(players_mbuf_t);

    logger("[S] Sending players");
    mqueue_put(player->connection->mqueueptr, s2c_mbuf);
}

void c_send_move(enum keyboard last_key) {
    mbuf_t mbuf;
    mbuf.msg.type = MSG_MOVE_PLAYER;
    mbuf.msg.size = sizeof(enum keyboard);
    if ((mbuf.payload = malloc(mbuf.msg.size)) == NULL) {
        panic("[C] Error allocating movement buffer!");
    }
    memcpy(mbuf.payload, &last_key, mbuf.msg.size);

    mqueue_put(&c2s_queue, mbuf);
}
