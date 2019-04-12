// vim: sw=4 ts=4 et :

#include "entity.h"

entity_t* entities[MAX_ENTITIES];
size_t entities_len = 1;

size_t entity_add(entity_t* e) {
    entities[entities_len] = e;
    return entities_len++;
}

size_t entity_create(enum stuff type, size_t y, size_t x) {
    entity_t* entity = (entity_t *)calloc(1, 
        sizeof(struct entity) +
        sizeof(struct creature_context)
    );
    if (entity == NULL) {
        panic("Cannot allocate entity!");
    }
    struct creature_context* creature_ctx = (struct creature_context*)(
        (char*) entity + sizeof(struct entity)
    );

    entity->y = y;
    entity->x = x;
    entity->stuff_type = type;
    entity->color = L_WHITE; // TODO set on the type basis

    entity->type = CREATURE;

    entity->context = creature_ctx;
    entity->player_context = NULL;

    creature_ctx->bt_root = &guard_behaviour; // TODO set on the type basis
    creature_ctx->bt_current = NULL;

    void* bt_context = creature_ctx->bt_context = calloc(
            1, guard_behaviour.context_size // TODO set on the type basis
            );

    // TODO do this dynamically (perhaps with BTree traversal)
    square_move_init(entity,
            (void*)(((char *)bt_context) +
                guard_behaviour.child ->u.composite.children[0]
                ->u.decorator.child ->u.leaf.u.offset
                ),
            NULL);

    return entity->id = entity_add(entity);
}

void c_receive_entities(entities_mbuf_t* mbuf) {
    if (! mbuf) return;

    char *buffer = (char *)(mbuf->entities);

#define NEW_ENTITY ((entity_t *)buffer)
#define OLD_ENTITY (entities[curr])
#define NEW_PLAYER ((struct player_context *)(buffer + sizeof(entity_t)))
#define OLD_PLAYER (OLD_ENTITY->player_context)

    while (mbuf->ecount) {
        size_t curr = NEW_ENTITY->id;
        if (curr >= entities_len) entities_len = curr + 1;

        if (! OLD_ENTITY) {
            entity_t* entity = (entity_t *)calloc(1, 
                    sizeof(struct entity) +
                    sizeof(struct creature_context) +
                    (
                     NEW_ENTITY->type == PLAYER ?
                     sizeof(struct player_context) : 0
                    )
                    );

            if (entity == NULL) {
                panic("[C] Error allocating entity for receive!");
            }

            entity->context = (struct creature_context *)(
                    (char *)entity + sizeof(struct entity)
                    );

            if (NEW_ENTITY->type == PLAYER) {
                entity->player_context = (struct player_context *) (
                        (char *)entity->context +
                        sizeof(struct creature_context)
                        );

                players[NEW_PLAYER->id] = entity;
                if (mbuf->self == 0) {
                    player_self = NEW_PLAYER->id;
                }
                if (NEW_PLAYER->id + 1 > players_len) {
                    players_len = NEW_PLAYER->id + 1;
                }
            } else {
                entity->player_context = NULL;
            }

            OLD_ENTITY = entity;
        }

        OLD_ENTITY->type       = NEW_ENTITY->type;
        OLD_ENTITY->x          = NEW_ENTITY->x;
        OLD_ENTITY->y          = NEW_ENTITY->y;
        OLD_ENTITY->stuff_type = NEW_ENTITY->stuff_type;
        OLD_ENTITY->color      = NEW_ENTITY->color;

        if (NEW_ENTITY->type == PLAYER) {
            if (! OLD_ENTITY->player_context) {
                panic("[C] There is no player_context for entity!");
            }

            OLD_PLAYER->id        = NEW_PLAYER->id;
            OLD_PLAYER->ready     = NEW_PLAYER->ready;
            OLD_PLAYER->connected = NEW_PLAYER->connected;
            OLD_PLAYER->start     = NEW_PLAYER->start;
            memcpy(OLD_PLAYER->nickname, NEW_PLAYER->nickname,
                    PLAYER_NAME_MAXLEN);

            buffer += sizeof(struct player_context);
            mbuf->pcount--;
        }

        buffer += sizeof(entity_t);
        mbuf->ecount--;
        if (mbuf->self >= 0) {
            mbuf->self--;
        }
    }

#undef NEW_ENTITY
#undef OLD_ENTITY
#undef NEW_PLAYER
#undef OLD_PLAYER
}

void s_send_entities_unsafe(entity_t* player, size_t ecount, size_t pcount,
        size_t* ids) {
    if (! player || ! ids) {
        panic("Invalid s_send_entities_unsafe() pointers!");
    }

    if (! player->player_context->connected) {
        return;
    }

    size_t size = sizeof(entities_mbuf_t) + sizeof(entity_t) * ecount +
        sizeof(struct player_context) * pcount;
    entities_mbuf_t* msg = (entities_mbuf_t *)malloc(size);
    if (msg == NULL) {
        panic("Error allocating buffer in s_send_entities_unsafe()!");
    }

    msg->ecount = ecount;
    msg->pcount = pcount;
    msg->self = -1;

    char *buffer = (char *)(msg->entities);

    size_t i = 0;
    while (ids[i]) {
        if (ids[i] >= entities_len) {
            panic("Invalid id specified for s_send_entities_unsafe()!");
        }

        memcpy(buffer, entities[ids[i]], sizeof(entity_t));

        buffer += sizeof(entity_t);
        ecount--;

        if (entities[ids[i]]->type == PLAYER) {
            if (entities[ids[i]]->player_context->connection ==
                    player->player_context->connection) {
                msg->self = i;
            }

            memcpy(buffer, entities[ids[i]]->player_context,
                    sizeof(struct player_context));
            buffer += sizeof(struct player_context);
            pcount--;
        }

        i++;

        if (ecount == 0) {
            break;
        }
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
    size_t* ids = (size_t*)malloc(sizeof(size_t) * entities_len);
    if (ids == NULL) {
        panic("Error allocating ids[]!");
    }

    for (size_t i = 1; i < entities_len; i++) {
        ids[i - 1] = i;
    }
    ids[entities_len - 1] = 0;

    s_send_entities_unsafe(player, entities_len - 1, players_len, ids);

    free(ids);
}

