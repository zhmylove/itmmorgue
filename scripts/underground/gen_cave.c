// vim: sw=4 ts=4 et :
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "gen_underground.h"

/*
 * Recursively encases specified zone in stone.
 */
void encase_zone(level_t *l, coords_t a, int zone) {
    if (!VALID_COORDS(l, a)) return;
    if (!is_floor(AT(l, a.x, a.y, a.z))) return;
    if (l->zone_info.map[a.x + a.y*l->w ] != zone) return;

    l->zone_info.map[a.x + a.y*l->w] = 0;
    AT(l, a.x, a.y, a.z) = '#';
    l->zone_info.zones[zone-1].size--;

    if (l->zone_info.zones[zone-1].size == 0) {
        // Should also update zone info map values
        // and remove zone from zone array
        return;
    }

    for (int i = 0; i < 16; i += 2)
        encase_zone(l, (coords_t){a.x+dirs[i], a.y+dirs[i+1], a.z}, zone);
}

/*
 * Encases all zones that are too small.
 */
void encase_small_zones(level_t *l, int threshold) {
    for(size_t i = 0; i < l->zone_info.count; ++i) {
        if(l->zone_info.zones[i].size < threshold) {
            encase_zone(l, l->zone_info.zones[i].at, i+1);
        }
    }
}

/*
 * Populates zone info by floodfill-testing empty spaces.
 */
void zone_test(level_t *l, int z) {
    int zone;
    ssize_t rtm_size = l->w * l->h * sizeof(int);

    l->zone_info.count = 0;
    l->zone_info.map = realloc(l->zone_info.map, rtm_size);
    memset(l->zone_info.map, 0, rtm_size);

    zone = 1;

    for(int j = 0; j < l->h; ++j) {
        for(int i = 0; i < l->w; ++i) {
            if(AT(l, i, j, z) == '.' && l->zone_info.map[i + j*l->w] == 0) {
                add_zone(l, (struct coords){i, j, z});
                zone_flood_fill(l, (struct coords){i, j, z}, zone++);
            }
        }
    }

}

/*
 * Smoothes wall formations at specified z-level.
 */
void smooth_pass(level_t *l, int z, int close_threshold, int open_threshold) {
    level_t temp;
    temp.map = NULL;

    if (z < 0 || z >= l->d) return;

    copy_level(l, &temp);

    for (int j = 0; j < l->h; ++j ) {
        for (int i = 0; i < l->w; ++i) {
            if (AT(l, i, j, z) != '#' && AT(l, i, j, z) != '.') continue;
            if (i == 0 || j == 0 || i == l->w-1 || j == l->h-1 )
                continue;

            int c = 0;

            for (int k = 0; k < 16; k += 2)
                if (is_floor(AT(l, i+dirs[k], j+dirs[k+1], z))) ++c;

            if (c >= open_threshold) AT(&temp, i, j, z) = '.';
            if (c <= close_threshold) AT(&temp, i, j, z) = '#';
        }
    }

    copy_level(&temp, l);
}

void simple_noise(level_t *l, int wall_rate) {
    for (int k = 0; k < l->d; ++k) {
        for (int j = 0; j < l->h; ++j ) {
            for (int i = 0; i < l->w; ++i) {
                if ((rand() % 100) > wall_rate) {
                    AT(l, i, j, k) = '#';
                } else {
                    AT(l, i, j, k) = '.';
                }
            }
        }
    }
}

/*
 * Returns suitable (random) stair placement on specified z-level.
 */
coords_t place_stairs(level_t *l, int z, char down) {
    int try = 1000000;
    int x, y;

    while (try--) {
        x = rand() % l->w;
        y = rand() % l->h;
        if (AT(l, x, y, z) == '.') {
            AT(l, x, y, z) = (down ? '>' : '<');
            return (coords_t){ x, y, z };
        }
    }

    // If everything is walled, let's forcibly make a staircase
    for (int k = 0; k < 8; ++k) {
        AT(l, x+dirs[2*k], y+dirs[2*k+1], z) = '.';
    }
    AT(l, x,y,z) = down ? '>' : '<';
    return (coords_t){ x, y, z};
}

/*
 * Builds a corridor between two positions.
 */
int do_corridor(level_t *l, coords_t a, coords_t b) {
    int tx = a.x, ty = a.y;
    int dx, dy;
    if (!VALID_COORDS(l, a) || !VALID_COORDS(l, b))
        return 1;

    // 2d corridors only
    if (a.z != b.z || (a.x == b.x && a.y == b.y))
        return 2;

    dx = b.x - a.x > 0 ? 1 : -1;
    dy = b.y - a.y > 0 ? 1 : -1;

    while (tx != b.x || ty != b.y) {
        if (abs(tx - b.x) >= abs(ty - b.y)) {
            tx += dx;
        } else {
            ty += dy;
        }

        if (!is_floor(AT(l, tx, ty, a.z))) {
            AT(l, tx, ty, a.z) = '.';
        }
    }

    return 0;
}

/*
 * Generates a cave system at specified z-level.
 */
int gen_cave(level_t *l, int z, coords_t stairs_pos, int openness) {
    coords_t downstairs;

    if (!VALID_COORDS(l, stairs_pos)) return 1;

    simple_noise(l, 40);

    for (int k = z; k < l->d; ++k) {
        AT(l, stairs_pos.x, stairs_pos.y, k) = '<';
        smooth_pass(l, k, 1, 4);

        zone_test(l, k);
        encase_small_zones(l, 50);

        downstairs = place_stairs(l, z, 1);

        if (ZONE_AT(l, stairs_pos.x, stairs_pos.y)
                != ZONE_AT(l, downstairs.x, downstairs.y)) {
            do_corridor(l, stairs_pos, downstairs);
        }

        for (int i = 0; i < openness; ++i) {
            smooth_pass(l, k, 2, 5);
        }

        stairs_pos = downstairs;
        stairs_pos.z = k + 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    int err;
    level_t *l = read_level(0);

    srand(time(NULL));

    coords_t upstairs = (coords_t) {
        rand() % l->w,
        rand() % l->h,
        0
    };

    if ((err = gen_cave(l, 0, upstairs, 2))) {
        fprintf(stderr, "failed to generate caves");
        return err;
    }
    write_level(l, 1);
    return EXIT_SUCCESS;
}
