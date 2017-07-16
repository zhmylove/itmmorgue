// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "windows.h"
#include "stuff.h"

tile_t *c_curr;
level_t *c_levels = NULL;
size_t c_levels_len = 0;
size_t c_levels_curr = 0;

void c_level_add(level_t *level) {
    for (size_t i = 0; i < c_levels_len; i++) {
        if (c_levels[i].id == level->id) {
            c_curr = c_levels[i].area;
            c_levels_curr = i;

            return;
        }
    }

#define CURR (c_levels[c_levels_len])
    if ((c_levels = (level_t *)realloc(c_levels,
                (c_levels_len + 1) * sizeof(level_t))) == NULL) {
        panic("[C] Error allocating new level buffer!");
    }

    CURR.id = level->id;
    strncpy(CURR.name, level->name, MAX_LEVEL_NAME);
    CURR.max_y = level->max_y;
    CURR.max_x = level->max_x;
    CURR.size = CURR.max_y * CURR.max_x;
    if ((CURR.area = (tile_t *)malloc(sizeof(tile_t) * CURR.size)) == NULL) {
        panic("[C] Error allocating new level area buffer!");
    }
    memset(CURR.area, '\0', sizeof(tile_t) * CURR.size);

    c_curr = CURR.area;
#undef CURR

    c_levels_curr = c_levels_len++;
}

static inline size_t tilepos(uint16_t y, uint16_t x) {
    return y * c_levels[c_levels_curr].max_x + x;
}

void tile_destroy(size_t index) {
    if (c_curr[index].top == S_NONE) {
        return;
    }

    tile_t *curr = c_curr[index].underlying;

    while (curr != NULL) {
        tile_t *to_destroy = curr;
        curr = curr->underlying;
        free(to_destroy);
    }

    memset(c_curr + index, '\0', sizeof(tile_t));
}

void c_area_update(size_t ngroups, tileblock_t *tileblock) {
    while (ngroups-- > 0) {
        uint32_t count = tileblock->count;
        uint32_t zcount = tileblock->zcount;

        for (uint32_t i = 0; i < count; i++) {
            size_t index = tilepos(tileblock->tiles->y, tileblock->tiles->x);

            tile_destroy(index);

            for (uint32_t j = 0; j < zcount; j++) {
                tile_t *updates = tileblock->tiles;

                if (c_curr[index].top == S_NONE) {
                    c_curr[index] = *updates;
                    tileblock = (tileblock_t*)((char *)tileblock +
                            sizeof(tile_t));
                    continue;
                }

                tile_t *unter;
                if ((unter = (tile_t *)malloc(sizeof(tile_t))) == NULL) {
                    panic("[C] Error allocating tile_t!");
                }
                *unter = c_curr[index];

                unter->underlying = NULL;
                c_curr[index] = *updates;
                c_curr[index].underlying = unter;
                tileblock = (tileblock_t*)((char *)tileblock + sizeof(tile_t));
            }
        }

        tileblock = (tileblock_t*)((char *)tileblock + sizeof(tileblock_t) -
                sizeof(tile_t));
    }
}

// TODO need to obtain **area from server
void area_init() {
    level_t lvl;
    lvl.id = 0x13;
    strcpy(lvl.name, "375");
    lvl.max_y = 13;
    lvl.max_x = 66;

    c_level_add(&lvl);

    tile_t t1;
    t1.top = S_PLAYER;
    t1.color = L_YELLOW;
    t1.x = 10;
    t1.y = 10;

    tile_t t2;
    t2.top = S_PLAYER;
    t2.color = D_BLUE;
    t2.x = 20;
    t2.y = 10;

    tile_t t3;
    t3.top = S_GRASS;
    t3.color = L_GREEN;
    t3.x = 30;
    t3.y = 10;

    tile_t t4;
    t4.top = S_PLAYER;
    t4.color = L_RED;
    t4.x = 30;
    t4.y = 10;

    tileblock_t *tbl;
    if ((tbl = (tileblock_t *)malloc(
                    sizeof(tile_t) * 4 +
                    (sizeof(tileblock_t) - sizeof(tile_t)) * 2
                    )) == NULL) {
        panic("Error creation 375!");
    }
    tbl->count = 2;
    tbl->zcount = 1;

    char *curr = (char*)&(tbl->tiles);
    memcpy(curr + sizeof(tile_t) * 0, &t1, sizeof(tile_t));
    memcpy(curr + sizeof(tile_t) * 1, &t2, sizeof(tile_t));

    tileblock_t *tbl2 = (tileblock_t *)((char *)tbl + sizeof(tile_t) * 2 +
                    (sizeof(tileblock_t) - sizeof(tile_t)) * 1);
    tbl2->count = 1;
    tbl2->zcount = 2;

    curr = (char*)&(tbl2->tiles);
    memcpy(curr + sizeof(tile_t) * 0, &t3, sizeof(tile_t));
    memcpy(curr + sizeof(tile_t) * 1, &t4, sizeof(tile_t));

    warnf("%p %p", (void*)tbl, (void*)tbl2);

    c_area_update(2, tbl);

    free(tbl);
}

void draw_area() {
    if (c_curr == NULL) {
        return;
    }

    tile_t *curr = c_curr;
    for (size_t size = 0; size < c_levels[c_levels_curr].size; size++) {
        mvwaddch(W(W_AREA), curr[size].y, curr[size].x,
                S[curr[size].top] | color2attr(curr[size].color));
    }

    mvwprintw(W(W_AREA), 3, 1, "%s %d", _("Key:"), last_key);
    mvwprintw(W(W_AREA), 4, 1, "%s %d", _("Connection:"), server_connected);
    mvwprintw(W(W_AREA), 5, 1, "%s %s", _("Server address:"), server_address);
    mvwprintw(W(W_AREA), 6, 1, "%s %s", _("Nikname:"), nickname);
    mvwprintw(W(W_AREA), 7, 1, "%s %d", _("Protocol:"), PROTOCOL_VERSION);

    return;
}

