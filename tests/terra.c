// vim: ts=4 sw=4 et :
/* Made by: KorG */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
// TODO add to itmmorgue.h
#include <math.h>

/*
 * ' ' = nothing
 * '#' = wall
 * '_' = floor
 * '^' = tree
 * '.' = grass
 * '+' = door
 */
char *area = NULL;

#define CITY_NAMELEN 16
typedef struct city {
    enum city_size {
        CITY_SMALL,
        CITY_MEDIUM,
        CITY_LARGE
    } size;
    char name[CITY_NAMELEN];
} city_t;

// LOCAL STUFF BEGIN
#define _DEBUG
void panic(char *str) {
    fprintf(stderr, "%s\n", str);
    exit(2);
}
// LOCAL STUFF END

void terra_visualize(char *area, size_t max_y, size_t max_x) {
    if (area == NULL) {
        return;
    }

    for (size_t i = 0; i < max_y; i++) {
        write(1, area + i * max_x, max_x);
        write(1, "\n", 1);
    }
}

typedef struct subline {
    size_t length;    // length of the subline
    char *start;      // pointers to the sublines beginning
} subline_t;

typedef struct subarea {
    size_t count;     // number of the sublines
    subline_t *lines; // pointer to the sublines
} subarea_t;

subarea_t terra_subarea(char *area,
        size_t area_max_y, size_t area_max_x,
        size_t center_y, size_t center_x, 
        size_t max_y, size_t max_x /* symmetry */) {
    subarea_t rc;
    rc.count = 0;
    rc.lines = NULL;
    
    if ((rc.lines = (subline_t *)malloc(sizeof(subline_t) * max_y)) == NULL) {
        panic("Error allocating subline!");
    }

    // Taking borders into account
    // NB! these values are really min() and max() values, not the size! :)
    size_t s_min_y = center_y > max_y / 2 ? center_y - max_y / 2 : 0;
    size_t s_min_x = center_x > max_x / 2 ? center_x - max_x / 2 : 0;
    size_t s_max_y = center_y + max_y / 2 < area_max_y ?
        center_y + max_y / 2 : area_max_y;
    size_t s_max_x = center_x + max_x / 2 < area_max_x ?
        center_x + max_x / 2 : area_max_x;

    s_max_y += max_y & 1;
    s_max_x += max_x & 1;

    for (size_t j = 0, i = s_min_y; i < s_max_y; j++, i++) {
        rc.count++;

        rc.lines[j].length = s_max_x - s_min_x;
        rc.lines[j].start = area + area_max_x * i + s_min_x;
    }

    return rc;
}

size_t terra_place_building(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x, size_t square) {
    size_t rc = 0;

    int side = (int)(sqrt(square) / 2);
    size_t square_new = 0;
    size_t size_y = 3 + random() % (3) * random() % (side + 1);
    size_t size_x = 3 + random() % (3) * random() % (side + 1);

    while (square_new < square) {
        square_new = size_y * size_x;

        if (random() % 101 < 50) {
            size_y++;
        } else {
            size_x++;
        }
    }

    subarea_t sub = terra_subarea(area, max_y, max_x,
            center_y, center_x, size_y, size_x);

    fprintf(stderr, "%zu %zu", size_y, size_x);

    for (size_t i = 0; i < sub.count; i++) {
        for (size_t j = 0; j < sub.lines[i].length; j++) {
#define CURR (sub.lines[i].start[j])
            rc++; // we'll measure total square for forest

            // we'll place building over EVERYTHING
            // if (CURR != ' ') {
            //     continue;
            // }

            if (i == 0 || i == sub.count - 1 ||
                    j == 0 || j == sub.lines[i].length - 1) {
                CURR = '#';
            } else {
                CURR = '_';
            }
#undef CURR
        }
    }

    int door_placed = 0;
    int door_required = 1 + random() % 2; // up to 2 doors
    while (door_placed < door_required) {
        door_placed++;
    }

    return rc;
}

size_t terra_place_building_tiny(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 9);
}

size_t terra_place_building_small(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 12);
}

size_t terra_place_building_medium(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 16);
}

size_t terra_place_building_big(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 49);
}

size_t terra_place_building_large(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 64);
}

size_t terra_place_forest(subarea_t sub, size_t density) {
    size_t rc = 0;

    for (size_t i = 0; i < sub.count; i++) {
        for (size_t j = 0; j < sub.lines[i].length; j++) {
#define CURR (sub.lines[i].start[j])
            rc++; // we'll measure total square for forest

            if (CURR != ' ') {
                continue;
            }

            // use <33 values for density to walk through
            if (random() % 101 <= density) {
                CURR = '^';
            } else {
                CURR = '.';
            }
#undef CURR
        }
    }

    return rc;
}

int terra_create(char **area, size_t max_y, size_t max_x, city_t *cities,
        size_t cities_len, size_t forest) {
#define AREA (*area)

    int rc = 0;
    int ttl; // number of tries

    size_t square = max_y * max_x;

    srandomdev();
    srandom(random());

    // TODO fix sizeof(tile_t)
    if ((AREA = malloc(sizeof(char) * max_y * max_x)) == NULL) {
        panic("Error allocating terra!");
    }

    // First of all, initialize all with nothing
    for (size_t i = 0; i < max_y; i++) {
        for (size_t j = 0; j < max_x; j++) {
            AREA[i * max_x + j] = ' ';

            rc++;
        }
    }

    // Place forests
    size_t forest_square = 0;
    ttl = 64;
    while (forest_square < square * forest / 100) {
        if (ttl-- == 0) {
            panic("Forest: ttl expired!");
        }

        size_t center_y = random() % (max_y + 1);
        size_t center_x = random() % (max_x + 1);
        size_t size_y = random() % (1 + max_y * forest / 100);
        size_t size_x = random() % (1 + max_x * forest / 100);

        size_t density = 10 + random() % 20;

#ifdef _DEBUG
        fprintf(stderr,
                "Forest (%zu / %zu) %zu: center %zu.%zu size %zu.%zu\n", 
                forest_square, square, density,
                center_y, center_x,
                size_y, size_x);
#endif /* _DEBUG */

        // TODO what's with density ?
        forest_square += terra_place_forest(
                terra_subarea(AREA, max_y, max_x,
                    center_y, center_x, size_y, size_x), // subarea
                density);                                // density
    }

    // Place building
    terra_place_building_tiny(AREA, max_y, max_x, 10, 10);
    terra_place_building_small(AREA, max_y, max_x, 10, 25);
    terra_place_building_medium(AREA, max_y, max_x, 20, 10);
    terra_place_building_big(AREA, max_y, max_x, 20, 35);
    terra_place_building_large(AREA, max_y, max_x, 30, 40);

    //     // Place the cities
    //     // Maybe we need to write a function, that works with filled space?
    //     for (int i = 0; i < cities_len; i++) {
    //         if (ttl-- <= 0) {
    //             return -1;
    //         }
    // 
    //         size_t center = random() % (max_y * max_x + 1);
    //         for (ttl = 64; ttl >= 0; ttl--) {
    //         }
    // 
    //         if (ttl <= 0) {
    //             return -2;
    //         }
    //     }

    // // Fill remaining with grass
    // for (int i = 0; i < max_y * max_x; i++) {
    //     if (AREA[i] == ' ') {
    //         AREA[i] = '.';
    //     }
    // }
#undef AREA

    return 0;
}

int main(int argc, char *argv[]) {   

    size_t max_y = 55;
    size_t max_x = 155;
    size_t forest = 75; // average percent

#define CITIES_LEN 2

    city_t cities[CITIES_LEN] = {
        { // city #1
            CITY_SMALL, // size
            "Adun"      // name
        },
        { // city #2
            CITY_SMALL, // size
            "Amon"      // name
        }
    };

    /* 
     * Generates terrain
     *
     * area       : pointer to area pointer
     * max_y      : vertical dimension
     * max_x      : horizontal dimension
     * cities     : pointer to required cities array
     * cities_len : number of cities in the array
     * forest     : density of forest in percent (0..100)
     *
     * ret        : negative on error, used space otherwise
     */
    terra_create(&area, max_y, max_x, cities, CITIES_LEN, forest);

    terra_visualize(area, max_y, max_x);

    (void)argc; (void)argv;
    return 0;
}
