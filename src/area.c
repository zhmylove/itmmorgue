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

size_t lvltilepos(uint16_t max_x, uint16_t y, uint16_t x) {
    return y * max_x + x;
}

size_t tilepos(uint16_t y, uint16_t x) {
    return lvltilepos(c_levels[c_levels_curr].max_x, y, x);
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

void draw_area() {
    if (levels_count == 0) { /* Before START_GAME */
        mvwprintw(W(W_AREA), 1, 1, "%s", _("Connected players:"));
        for (size_t i = 0; i < players_len; i++) {
            mvwaddch(W(W_AREA), 2 + i, 1,
                    players[i]->player_context->ready ? '+' : '-');
            mvwprintw(W(W_AREA), 2 + i, 3, "%s",
                    players[i]->player_context->nickname);
            mvwprintw(W(W_AREA), windows[W_AREA].max_y - 2, 1, "%s",
                    _("Write !start or !s to chat to begin"));
        }
    }

    if (c_curr == NULL) {
        return;
    }

    /* Calculate top_y and top_x for area */
    // TODO do we really need to check conf() every tick?
    ssize_t top_y = 0, top_x = 0, camera = conf("player_camera").ival;
    if (players_len > 0) {
#define AREA (windows[W_AREA])
#define ME (*(players[player_self]))
        if (camera == 1) {
            // Centre view
            top_y = ME.y - AREA.max_y / 2;
            top_x = ME.x - AREA.max_x / 2;
        } else if (camera == 2) {
            // Part-screen movement
            top_y = ((ME.y - (AREA.max_y / 8)) / (AREA.max_y * 6 / 8)) *
                (AREA.max_y * 6 / 8);
            top_x = ((ME.x - (AREA.max_x / 8)) / (AREA.max_x * 6 / 8)) *
                (AREA.max_x * 6 / 8);
        } else {
            // Another view
        }

        // Corrections
        if (top_y <  0) top_y = 0;
        if (top_x <  0) top_x = 0;
        // TODO do all of our levels has such big size?
        // TODO change c_levels to current_level_pointer
        if (top_y >= c_levels->max_y - AREA.max_y) top_y =
            c_levels->max_y - AREA.max_y;
        if (top_x >= c_levels->max_x - AREA.max_x) top_x =
            c_levels->max_x - AREA.max_x;
    }

    tile_t *curr = c_curr;
    for (register ssize_t y = 0; y < AREA.max_y; y++) {
        register size_t pos = tilepos(y + top_y, top_x);
        for (register ssize_t x = 0; x < AREA.max_x; x++, pos++) {
            mvwaddch(W(W_AREA), y, x,
                    S[curr[pos].top] | color2attr(curr[pos].color));
        }
    }

    /* Draw debug variables */
    mvwprintw(W(W_AREA), 3, 1, "%s %d", _("Key:"), last_key);
    mvwprintw(W(W_AREA), 4, 1, "%s %d", _("Connection:"), server_connected);
    mvwprintw(W(W_AREA), 5, 1, "%s %s", _("Server address:"), server_address);
    mvwprintw(W(W_AREA), 6, 1, "%s %s", _("Nikname:"), nickname);
    mvwprintw(W(W_AREA), 7, 1, "%s %d", _("Protocol:"), PROTOCOL_VERSION);
    mvwprintw(W(W_AREA), 8, 1, "%s %d", _("Player ID:"), player_self);
    mvwprintw(W(W_AREA), 9, 1, "%s %d x %d", _("Top YxX:"), top_y, top_x);
    mvwprintw(W(W_AREA), 10, 1, "%s %d x %d", _("Win YxX:"), 
            windows[W_AREA].max_y, windows[W_AREA].max_x);
    if (players_len > 0) {
        mvwprintw(W(W_AREA), 11, 1, "%s %d x %d", _("My YxX:"), ME.y, ME.x);
    }
#undef ME
#undef AREA

    /* Draw the entities */
    // TODO reduce the area
    for (size_t i = 1; i < entities_len; i++) {
        if (entities[i] == NULL) continue;

        mvwaddch(W(W_AREA), entities[i]->y - top_y, entities[i]->x - top_x,
                S[entities[i]->stuff_type] | color2attr(entities[i]->color));
    }

    /* Draw the players */
    for (size_t i = 0; i < players_len; i++) {
        mvwaddch(W(W_AREA), players[i]->y - top_y, players[i]->x - top_x,
                S[S_PLAYER] | color2attr(players[i]->color));
    }

    return;
}

