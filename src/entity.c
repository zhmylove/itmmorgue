#include "entity.h"

entity_t* entities[MAX_ENTITIES];
uint32_t entities_len = 1;

uint32_t entity_add(entity_t* e){
    entities[entities_len] = e;
    return entities_len++;
}


void c_receive_entities_full(entity_full_mbuf_t* mbuf){
    if (!mbuf || mbuf->players_len >= MAX_PLAYERS) return;

    player_self = mbuf->self;

    /* Maybe we shouldn't send connection, etc. */
    for (players_len = 0; players_len < mbuf->players_len; players_len++) {
        players[players_len] = mbuf->players[players_len];
    }
}


void s_send_entities_full(entity_t* player){
    entities_full_mbuf_t* ents_mbuf;

    if (! player->player_context.connected) {
        return;
    }

    ents_mbuf = (entities_full_mbuf_t *)malloc(sizeof(entities_full_mbuf_t)
        +((entities_len)*sizeof(entity_t)));
    if (NULL == ents_mbuf) {
       panic("Error creating players_full_mbuf for send!");
    }
    for (size_t i = 1; i < entities_len; i++) {
        if (
            entities[i]->type == PLAYER
            && player->player_context.connection
                == entities[i]->player_context.connection
        ) {
            ents_mbuf->self = i;
        }
        ents_mbuf->entities[i] = entities[i];
    }
    ents_mbuf->players_len = players_len;

    mbuf_t s2c_mbuf;
    s2c_mbuf.payload = (void *)ents_mbuf;
    s2c_mbuf.msg.type = MSG_PUT_PLAYERS_FULL;
    s2c_mbuf.msg.size = sizeof(entities_full_mbuf_t);

    logger("[S] Sending players full");
    mqueue_put(player->connection->mqueueptr, s2c_mbuf);
}