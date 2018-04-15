// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "server.h"
#include "stuff.h"
#include "entity.h"

/// player_t players[MAX_PLAYERS];
entity_t* players2[MAX_PLAYERS];
size_t players2_id[MAX_PLAYERS];
size_t players_len = 0;
size_t player_self = 0;
size_t players_total = 0;

/*
 * TODO implement speed, area_check, npc_check and handle other stuff.
 */
void player_move(player_move_t *move) {
    size_t id = move->player_id;

    switch (move->direction) {
        case K_MOVE_LEFT:
            players2[id]->x--;
            break;
        case K_MOVE_RIGHT:
            players2[id]->x++;
            break;
        case K_MOVE_UP:
            players2[id]->y--;
            break;
        case K_MOVE_DOWN:
            players2[id]->y++;
            break;
        case K_MOVE_LEFT_UP:
            players2[id]->y--;
            players2[id]->x--;
            break;
        case K_MOVE_RIGHT_UP:
            players2[id]->y--;
            players2[id]->x++;
            break;
        case K_MOVE_LEFT_DOWN:
            players2[id]->y++;
            players2[id]->x--;
            break;
        case K_MOVE_RIGHT_DOWN:
            players2[id]->y++;
            players2[id]->x++;
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
    players2[players_len] = player;
    players2_id[players_len] = entity_add(player);

    return players_len++;
}

/// void c_receive_players_full(players_full_mbuf_t *mbuf) {
///     if (!mbuf || mbuf->players_len >= MAX_PLAYERS) return;
/// 
///     player_self = mbuf->self;
/// 
///     /* Maybe we shouldn't send connection, etc. */
///     for (players_len = 0; players_len < mbuf->players_len; players_len++) {
///         players[players_len] = mbuf->players[players_len];
///     }
/// }
/// 
/// void c_receive_players(players_mbuf_t *mbuf) {
///     if (!mbuf || mbuf->players_len >= MAX_PLAYERS) return;
/// 
///     player_self = mbuf->self;
/// 
///     for (players_len = 0; players_len < mbuf->players_len; players_len++) {
///         players[players_len].color = mbuf->players[players_len].color ;
///         players[players_len].y     = mbuf->players[players_len].y     ;
///         players[players_len].x     = mbuf->players[players_len].x     ;
///     }
/// }
/// 
void s_send_players_full(entity_t* player) {
    uint32_t* ids = (uint32_t*)malloc(sizeof(uint32_t) * players_len + 1);
    if (ids == NULL) {
        panic("[S] Error allocating ids[]!");
    }

    for (uint32_t i = 0; i < players_len; i++) {
        ids[i] = players2_id[i];
    }
    ids[players_len] = 0;

    s_send_entities_unsafe(player, players_len, players_len, ids);

    free(ids);
}

/// void s_send_players_full(player_t *player) {
///     players_full_mbuf_t* players_mbuf;
/// 
///     if (! player->connected) {
///         return;
///     }
/// 
///     if ((players_mbuf = (players_full_mbuf_t *)malloc(
///                     sizeof(players_full_mbuf_t)
///                     )) == NULL) {
///         panic("Error creating players_full_mbuf for send!");
///     }
///     for (size_t i = 0; i < players_len; i++) {
///         if (player->connection == players[i].connection) {
///             players_mbuf->self = i;
///         }
///         players_mbuf->players[i] = players[i];
///     }
///     players_mbuf->players_len = players_len;
/// 
///     mbuf_t s2c_mbuf;
///     s2c_mbuf.payload = (void *)players_mbuf;
///     s2c_mbuf.msg.type = MSG_PUT_PLAYERS_FULL;
///     s2c_mbuf.msg.size = sizeof(players_full_mbuf_t);
/// 
///     logger("[S] Sending players full");
///     mqueue_put(player->connection->mqueueptr, s2c_mbuf);
/// }
/// 
/// void s_send_players(player_t *player) {
///     players_mbuf_t* players_mbuf;
/// 
///     if (! player->connected) {
///         return;
///     }
/// 
///     if ((players_mbuf = (players_mbuf_t *)malloc(sizeof(players_mbuf_t))) ==
///             NULL) {
///         panic("Error creating players_mbuf for send!");
///     }
///     for (size_t i = 0; i < players_len; i++) {
///         if (player->connection == players[i].connection) {
///             players_mbuf->self = i;
///         }
///         players_mbuf->players[i].color = players[i].color;
///         players_mbuf->players[i].y     = players[i].y;
///         players_mbuf->players[i].x     = players[i].x;
///     }
///     players_mbuf->players_len = players_len;
/// 
///     mbuf_t s2c_mbuf;
///     s2c_mbuf.payload = (void *)players_mbuf;
///     s2c_mbuf.msg.type = MSG_PUT_PLAYERS;
///     s2c_mbuf.msg.size = sizeof(players_mbuf_t);
/// 
///     logger("[S] Sending players");
///     mqueue_put(player->connection->mqueueptr, s2c_mbuf);
/// }

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
