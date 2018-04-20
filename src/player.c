// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "server.h"
#include "stuff.h"
#include "entity.h"

entity_t* players[MAX_PLAYERS];
size_t players_id[MAX_PLAYERS];
size_t players_len = 0;
size_t player_self = 0;
size_t players_total = 0;

/*
 * TODO implement speed, area_check, npc_check and handle other stuff.
 * TODO re-implement this function with entity_move()
 */
void player_move(player_move_t *move) {
    size_t id = move->player_id;

    switch (move->direction) {
        case K_MOVE_LEFT:
            players[id]->x--;
            break;
        case K_MOVE_RIGHT:
            players[id]->x++;
            break;
        case K_MOVE_UP:
            players[id]->y--;
            break;
        case K_MOVE_DOWN:
            players[id]->y++;
            break;
        case K_MOVE_LEFT_UP:
            players[id]->y--;
            players[id]->x--;
            break;
        case K_MOVE_RIGHT_UP:
            players[id]->y--;
            players[id]->x++;
            break;
        case K_MOVE_LEFT_DOWN:
            players[id]->y++;
            players[id]->x--;
            break;
        case K_MOVE_RIGHT_DOWN:
            players[id]->y++;
            players[id]->x++;
            break;
        default:
            panic("[S] invalid move direction!");
    }
}

size_t player_init(enum colors color, char *nickname,
        connection_t *connection) {

    entity_t* player = (entity_t *)malloc(
        sizeof(struct entity) +
        sizeof(struct creature_context) +
        sizeof(struct player_context)
    );
    if( NULL == player ){
        panic("Cannot allocate player!");
    }
    struct creature_context* creature_ctx = (struct creature_context*)(
        (char*) player + sizeof(struct entity)
    );
    struct player_context* player_ctx = (struct player_context*) (
        (char*)creature_ctx + sizeof(struct creature_context)
    );

    player->type = PLAYER;
    player->stuff_type = S_PLAYER;
    player->color = color;

    player->context = creature_ctx;
    player->player_context = player_ctx;

    player_ctx->connection = connection;
    player_ctx->ready = 0;
    player_ctx->start = 0;
    if (0 != pthread_mutex_init(&player_ctx->ev_queue.event_mutex,
                NULL)) {
        panic("Cannot initialize event queue mutex!");
    }
    strncpy(player_ctx->nickname, nickname, CHAT_NICK_MAXLEN);
    creature_ctx->bt_root = NULL;
    creature_ctx->bt_current = NULL;

    if (start != 3) {
        /*
         * We don't need to set y & x on dead players.
         * They'll be restored during NICKNAME reception.
         */
        player->y = 8;
        player->x = 48;
    }
    players[players_len] = player;
    players_id[players_len] = entity_add(player);

    return players_len++;
}

void s_send_players_full(entity_t* player) {
    size_t* ids = (size_t*)malloc(sizeof(size_t) * players_len + 1);
    if (ids == NULL) {
        panic("[S] Error allocating ids[]!");
    }

    for (size_t i = 0; i < players_len; i++) {
        ids[i] = players_id[i];
    }
    ids[players_len] = 0;

    s_send_entities_unsafe(player, players_len, players_len, ids);

    free(ids);
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
