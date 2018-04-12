// vim: sw=4 ts=4 et :

#include "entity.h"

entity_t* entities[MAX_ENTITIES];
uint32_t entities_len = 1;

uint32_t entity_add(entity_t* e){
    entities[entities_len] = e;
    return entities_len++;
}

/// void c_receive_entities_full(entity_full_mbuf_t* mbuf){
///     if (!mbuf || mbuf->players_len >= MAX_PLAYERS) return;
/// 
///     player_self = mbuf->self;
/// 
///     /* Maybe we shouldn't send connection, etc. */
///     for (players_len = 0; players_len < mbuf->players_len; players_len++) {
///         players[players_len] = mbuf->players[players_len];
///     }
/// }


/* 
 * Send ecount entities to the player. ids[] contains pcount of players.
 * Unsafe: performance in a loss of reliability
 */
void s_send_entities_unsafe(entity_t* player, size_t ecount, size_t pcount, 
        uint32_t* ids) {
    if (! player || ! ids) {
        panic("Invalid s_send_entities_unsafe() pointers!");
    }

    if (! player->player_context->connected) {
        return;
    }

    size_t size = sizeof(entities_mbuf_t) - sizeof(entity_t *) +
        sizeof(entity_t) * ecount + sizeof(struct player_context) * pcount;
    entities_mbuf_t* msg = (entities_mbuf_t *)malloc(size);
    if (msg == NULL) {
        panic("Error allocating buffer in s_send_entities_unsafe()!");
    }

    msg->ecount = ecount;
    msg->pcount = pcount;

    char *buffer = (char *)msg + sizeof(entities_mbuf_t) - sizeof(entity_t *);

    size_t i = 0;
    while (*(ids + i)) {
        if (*ids >= entities_len) {
            panic("Invalid id specified for s_send_entities_unsafe()!");
        }

        memcpy(buffer, entities[*(ids + i)], sizeof(entity_t));
        buffer += sizeof(entity_t);
        ecount--;

        if (entities[*(ids + i)]->type == PLAYER) {
            if (entities[*(ids + i)]->player_context->connection ==
                    player->player_context->connection) {
                msg->self = i;
            }

            memcpy(buffer, entities[*(ids + i)]->player_context,
                    sizeof(struct player_context));
            buffer += sizeof(struct player_context);
            pcount--;
        }

        i++;
    }

    if (ecount || pcount) {
        panic("Invalid ecount or pcount in s_send_entities_unsafe()!");
    }

    mbuf_t s2c_mbuf;
    s2c_mbuf.payload = (void *)msg; // will be automatically freed later on
    s2c_mbuf.msg.type = MSG_PUT_ENTITIES;
    s2c_mbuf.msg.size = size;

    logger("[S] Sending entities...");
    mqueue_put(player->player_context->connection->mqueueptr, s2c_mbuf);
}

void s_send_entities_full(entity_t* player) {
    uint32_t* ids = (uint32_t*)malloc(sizeof(uint32_t) * entities_len);
    if (ids == NULL) {
        panic("Error allocating ids[]!");
    }

    for (uint32_t i = 0; i < entities_len; i++) {
        ids[i] = i;
    }
    s_send_entities_unsafe(player, entities_len, players_len, ids);

    free(ids);
}

