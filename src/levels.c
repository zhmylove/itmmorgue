// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "server.h"
#include "stuff.h"
#include "generate.h"
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

    int x, y; // Iterators

    // Would panic on any problems
    char **text_level = terra_generate(HEAD.max_y, HEAD.max_x);

    tile_t buftile;
    for (y = 0; y < HEAD.max_y; y++) {
        for (x = 0; x < HEAD.max_x; x++) {
            switch (text_level[y][x]) {
                /* Can't be simplified with stuff.c => hardcode */
#define MAP(CHAR, TOP, COLOR) \
                case CHAR: buftile.top = TOP; buftile.color = COLOR; break;
                           MAP('#', S_WALL        , D_WHITE   );
                           MAP('.', S_GRASS       , D_GREEN   );
                           MAP('^', S_TREE        , L_GREEN   );
                           MAP(',', S_CITY        , L_BLACK   );
                           MAP('_', S_FLOOR       , D_WHITE   );
                           MAP('"', S_FIELD       , D_YELLOW  );
                           MAP('+', S_DOOR        , L_WHITE   );
#undef MAP
                default:
                           buftile.top = S_NONE;
                           buftile.color = L_BLACK;
            }
            buftile.x = x;
            buftile.y = y;
            buftile.underlying = NULL;

            // TODO Could be optimized
            HEAD.area[lvltilepos(HEAD.max_x, y, x)] = buftile;
        }
    }

    /* entities init */
    // TODO make this dynamically
    entity_create(S_SCROLL, 11, 26);
    entity_create(S_SCROLL, 11, 36);
    entity_create(S_SCROLL, 11, 46);

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
