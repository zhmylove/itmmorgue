// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "server.h"
#include "stuff.h"
#include "npc.h"
#include "bt_leaf.h"

level_t *levels;
size_t levels_count = 0;

#define LVL(id) (levels[id])
#define HEAD (LVL(levels_count))

void s_levels_init() {
    // TODO generate levels JIT
    // otherwise hardcode:

    if ((levels = (level_t *)malloc(sizeof(level_t) * 1)) == NULL) {
        panic("Error allocating levels!");
    }

    HEAD.id = 0x13;
    strcpy(HEAD.name, "375");
    HEAD.max_y = conf("level_height").ival;
    HEAD.max_x = conf("level_width").ival;
    HEAD.size = HEAD.max_y * HEAD.max_x;
    if ((HEAD.area = (tile_t *)malloc(sizeof(tile_t) * HEAD.size)) == NULL) {
        panic("Error allocating level area!");
    }

    // TODO realloc from the script
    FILE *gen;
    char gen_cmd[GEN_MAX];
    if (snprintf(gen_cmd, GEN_MAX, GEN_SH,
                conf("level_width").ival, conf("level_height").ival) < 0) {
        panic("Error formatting gen_cmd!");
    }
    if ((gen = popen(gen_cmd, "r")) == NULL) {
        panic("Error running Gen.sh!");
    }

    int ch, x = 0, y = 0;
    while ((ch = getc(gen)) != EOF) {
        // TODO parse ch
        tile_t buftile;
        switch ((char)ch) {
            case '\n':
                y++;
                x = 0;
                continue;
            case '^':
                buftile.top = S_TREE;
                buftile.color = L_GREEN;
                break;
            default:
                buftile.top = S_FLOOR;
                buftile.color = L_BLACK;
        }
        buftile.x = x;
        buftile.y = y;
        buftile.underlying = NULL;

        HEAD.area[lvltilepos(HEAD.max_x, y, x++)] = buftile;
    }

    if (! feof(gen)) {
        panic("Not EOF on Gen.sh!");
    }

    pclose(gen);

    /* entities init */
    // TODO fix entities and remove this stub entity
    entity_t* entity = (entity_t *)malloc(
        sizeof(struct entity) +
        sizeof(struct creature_context)
    );
    if (NULL == entity) {
        panic("Cannot allocate entity!");
    }
    struct creature_context* creature_ctx = (struct creature_context*)(
        (char*) entity + sizeof(struct entity)
    );

    entity->type = CREATURE;
    entity->y = 10;
    entity->x = 25;
    entity->stuff_type = S_SCROLL;
    entity->color = L_WHITE;

    entity->context = creature_ctx;
    entity->player_context = NULL;

    creature_ctx->bt_root = &guard_behaviour;
    creature_ctx->bt_current = NULL;

    void* bt_context = creature_ctx->bt_context = calloc(
            1, guard_behaviour.context_size
            );

    #define OFFSET(p, o) (((char*)(p))+(o))

    // TODO do this dynamically (perhaps with BTree traversal)
    square_move_init(entity, 
            (void*)(((char *)bt_context) +
                guard_behaviour.child ->u.composite.children[0]
                ->u.decorator.child ->u.leaf.u.offset
                ),
            NULL);

    entity->id = entity_add(entity);

    /* TODO end of stub entities */

    levels_count++;
}

void s_area_send(size_t level, entity_t *player) {
    tileblock_t *tbl;
    size_t size = LVL(level).max_y * LVL(level).max_x;

    loggerf("[S] s_area_send(%zu): %d x %d", level,
            LVL(level).max_y, LVL(level).max_x);

    /* "1" is because tileblocks are unneeded in the game:
     * we had decided to send only visible data (.top)
     * before we implemented tileblocks due to lack of
     * our memory...
     * Why don't we have panic() for malloc in our brains? :(
     */
    size_t wholesize = sizeof(tile_t) * size +
        (sizeof(tileblock_t) - sizeof(tile_t)) * 1;

    if ((tbl = (tileblock_t *)malloc(wholesize)) == NULL) {
        panic("Error creation tileblock for send!");
    }
    tbl->count = size;
    tbl->zcount = 1;

    char *curr = (char*)&(tbl->tiles);
    for (size_t i = 0; i < size; i++) {
        memcpy(curr + sizeof(tile_t) * i, &(LVL(level).area[i]),
                sizeof(tile_t));
    }

    mbuf_t s2c_mbuf;
    s2c_mbuf.payload = (void *)tbl;
    s2c_mbuf.msg.type = MSG_PUT_AREA;
    s2c_mbuf.msg.size = wholesize;

    loggerf("[S] Sending AREA: size=%zu", wholesize);
    mqueue_put(player->player_context->connection->mqueueptr, s2c_mbuf);
}

void s_level_send(size_t level, entity_t *player) {
    mbuf_t s2c_mbuf;
    level_t *lvl;
    if ((lvl = (level_t *)malloc(sizeof(level_t))) == NULL) {
        panic("Error allocating level mbuf!");
    }
    memcpy(lvl, &LVL(level), sizeof(level_t));
    s2c_mbuf.payload = (void *)lvl;
    s2c_mbuf.msg.type = MSG_PUT_LEVEL;
    s2c_mbuf.msg.size = sizeof(level_t);

    loggerf("[S] Sending LEVEL: size=%zu", sizeof(level_t));
    mqueue_put(player->player_context->connection->mqueueptr, s2c_mbuf);
}
