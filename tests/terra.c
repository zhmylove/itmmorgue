// vim: ts=4 sw=4 et :
/* Made by: KorG */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
// TODO add to itmmorgue.h
// TODO add -lm for Linux
#include <math.h>

// TODO HERE:
// 1. place cities
// 2. place roads
// 3. fix memory leaks (add free() for subarea_t, subline_t)


/*
 * ' ' = nothing
 * '#' = wall
 * '_' = floor
 * '^' = tree
 * '.' = grass
 * '+' = door
 */
char *area = NULL;

enum city_size {
    CITY_TINY,
    CITY_SMALL,
    CITY_MEDIUM,
    CITY_BIG,
    CITY_LARGE,
    CITY_SIZE
};

#define CITY_NAMELEN 16

typedef struct city {
    enum city_size size;
    char name[CITY_NAMELEN];
} city_t;

typedef struct subline {
    size_t length;    // length of the subline
    char *start;      // pointers to the sublines beginning
} subline_t;

typedef struct subarea {
    size_t count;     // number of the sublines
    size_t abs_min_y; // absolute coordinates of corners
    size_t abs_min_x;
    size_t abs_max_y;
    size_t abs_max_x;
    subline_t *lines; // pointer to the sublines
} subarea_t;

typedef struct xy {
    int y;
    int x;
} xy_t;

// TODO remove LOCAL STUFF BEGIN

#define _DEBUG
void panic(char *str) {
    fprintf(stderr, "%s\n", str);
    exit(2);
}
unsigned long long sysutime() {
    struct timeval tv; 

    if (gettimeofday(&tv, NULL) < 0) {
        panic("Unable to get system time!");
    }

    return tv.tv_sec * 1000000 + tv.tv_usec;
}
enum stuff {
    S_NONE            ,
    S_CITY            , // city outside buildings
    S_FLOOR           , // in building
    S_WALL            , // of building
    S_DOOR            , // of building
    S_TREE            , // in forest
    S_GRASS           , // in forest
    S_FIELD           , // in plains
    S_PLAYER          ,
    S_DOWNSTAIRS      ,
    S_UPSTAIRS        ,

    S_TRAP            ,
    S_FOOD            ,

    S_GOLD            ,
    S_SCROLL          ,
    S_BOOK            ,

    S_RING            ,
    S_WAND            ,
    S_SIZE
};

// TODO end remove LOCAL STUFF END

size_t terra_pos(size_t area_max_x, size_t y, size_t x) {
    return y * area_max_x + x;
}

int terra_is_walkable(char ch) {
    switch (ch) {
        case ' ':
        case '.':
        case '_':
            return 1;
        default:
            return 0;
    }

    return 0;
}

void terra_visualize(char *area, size_t max_y, size_t max_x) {
    if (area == NULL) {
        return;
    }

    for (size_t i = 0; i < max_y; i++) {
        write(1, area + i * max_x, max_x);
        write(1, "\n", 1);
    }
}

subarea_t terra_subarea_safe(char *area,
        size_t area_max_y, size_t area_max_x, size_t indent,
        size_t center_y, size_t center_x, 
        size_t max_y, size_t max_x /* symmetry */) {
    subarea_t rc;
    rc.count = 0;
    rc.lines = NULL;

    // TODO move below?
    if ((rc.lines = (subline_t *)malloc(sizeof(subline_t) * max_y)) == NULL) {
        panic("Error allocating subline!");
    }

    // Taking borders into account
    // NB! these values are really min() and max() values, not the size! :)
    size_t s_min_y = center_y > max_y / 2 ?
        center_y - max_y / 2 : 0;
    size_t s_min_x = center_x > max_x / 2 ?
        center_x - max_x / 2 : 0;
    size_t s_max_y = center_y + max_y / 2 < area_max_y ?
        center_y + max_y / 2 : area_max_y;
    size_t s_max_x = center_x + max_x / 2 < area_max_x ?
        center_x + max_x / 2 : area_max_x;

    s_max_y += max_y & 1;
    s_max_x += max_x & 1;

    // Safeness BEGIN
    if (area_max_y / 2 <= indent || area_max_x / 2 <= indent) {
        panic("Too big indent over area!");
    }

    // Move the borders
    if (s_min_y < indent) {
        s_max_y += indent - s_min_y;
        s_min_y = indent;
    }
    if (s_min_x < indent) {
        s_max_x += indent - s_min_x;
        s_min_x = indent;
    }
    if (s_max_y > area_max_y - indent) {
        s_min_y -= s_max_y - area_max_y + indent;
        s_max_y = area_max_y - indent;
    }
    if (s_max_x > area_max_x - indent) {
        s_min_x -= s_max_x - area_max_x + indent;
        s_max_x = area_max_x - indent;
    }
    if (s_min_y < indent || s_min_x < indent) {
        panic("Too big indent specified!");
    }
    // Safeness END

    for (size_t j = 0, i = s_min_y; i < s_max_y; j++, i++) {
        rc.count++;

        rc.lines[j].length = s_max_x - s_min_x;
        rc.lines[j].start = area + area_max_x * i + s_min_x;
    }

    // TODO CHECK
    rc.abs_min_y = s_min_y;
    rc.abs_min_x = s_min_x;
    rc.abs_max_y = s_max_y - 1;
    rc.abs_max_x = s_max_x - 1;

    return rc;
}

subarea_t terra_subarea(char *area,
        size_t area_max_y, size_t area_max_x,
        size_t center_y, size_t center_x,
        size_t max_y, size_t max_x /* symmetry */) {
    return terra_subarea_safe(area, area_max_y, area_max_x, 0,
            center_y, center_x, max_y, max_x /* symmetry */);
}

size_t terra_place_building(char *area, size_t area_max_y, size_t area_max_x,
        size_t center_y, size_t center_x, size_t square, subarea_t *retsub) {
    size_t rc = 0;

    int side = (int)(sqrt(square) / 2);
    size_t square_new = 0;
    size_t size_y = 3 + random() % (3) * random() % (side + 1);
    size_t size_x = 3 + random() % (3) * random() % (side + 1);

#ifdef _DEBUG
    fprintf(stderr, "SUBAREA: %zu x %zu\n", size_y, size_x);
#endif /* _DEBUG */

    while (square_new < square) {
        square_new = size_y * size_x;

        if (random() % 101 < 50) {
            size_y++;
        } else {
            size_x++;
        }
    }

    subarea_t sub;
    memset(&sub, 0, sizeof(sub));

    while (sub.abs_max_y - sub.abs_min_y < 2 ||
            sub.abs_max_x - sub.abs_min_x < 2) {
        // In sake of safety
        if (center_y < 1) {
            center_y = 1;
        }

        if (center_x < 1) {
            center_x = 1;
        }

        sub = terra_subarea_safe(area, area_max_y, area_max_x, 1,
                center_y, center_x, size_y, size_x);
    }

    for (size_t i = 0; i < sub.count; i++) {
        for (size_t j = 0; j < sub.lines[i].length; j++) {
#define CURR (sub.lines[i].start[j])
            rc++; // we'll measure total square for forest

            // we'll place building over EVERYTHING
            switch (CURR) {
                case '_':
                    continue;
                case '+':
                    CURR = '_'; // small easter-egg, never'll be found ;-)
                    continue;
                case '#':
                    break;
            }

            if (i == 0 || i == sub.count - 1 ||
                    j == 0 || j == sub.lines[i].length - 1) {
                CURR = '#';
            } else {
                CURR = '_';
            }
#undef CURR
        }
    }

    // Place some doors
    int door_placed = 0;
    int door_required = 1 + random() % 2; // up to 2 doors
    while (door_placed < door_required) {
        size_t X, Y; // absolute coordinates of potential door

        if (random() % 101 < 50) {
            // horizontal wall door
            Y = random() % 101 < 50 ? sub.abs_min_y : sub.abs_max_y;
            X = 1 + sub.abs_min_x + random() % (sub.abs_max_x - sub.abs_min_x);
        } else {
            // vertical wall door
            Y = 1 + sub.abs_min_y + random() % (sub.abs_max_y - sub.abs_min_y);
            X = random() % 101 < 50 ? sub.abs_min_x : sub.abs_max_x;
        }

#ifdef _DEBUG
        fprintf(stderr, "Door: %zu %zu (%zu %zu %zu %zu)\n", Y, X,
                sub.abs_min_y,
                sub.abs_min_x,
                sub.abs_max_y,
                sub.abs_max_x
               );
#endif /* _DEBUG */

        // Just skip the door if it's already here ;)
        if (area[terra_pos(area_max_x, Y, X)] == '+') {
            door_placed++;
            continue;
        }

        // TODO maybe extend this to check a direction:
        // kinda LEFT(grass) && RIGHT(floor) and vise versa

        if ( // yeah, vim's so crazy in indentation!
                // this shity code just takes some precautions
                (
                 (
                  terra_is_walkable(
                      area[terra_pos(area_max_x, Y, X - 1)]) &&
                  terra_is_walkable(
                      area[terra_pos(area_max_x, Y, X + 1)]) &&
                  ! terra_is_walkable(
                      area[terra_pos(area_max_x, Y - 1, X)]) &&
                  ! terra_is_walkable(
                      area[terra_pos(area_max_x, Y + 1, X)])
                 ) ||
                 (
                  terra_is_walkable(
                      area[terra_pos(area_max_x, Y - 1, X)]) &&
                  terra_is_walkable(
                      area[terra_pos(area_max_x, Y + 1, X)]) &&
                  ! terra_is_walkable(
                      area[terra_pos(area_max_x, Y, X - 1)]) &&
                  ! terra_is_walkable(
                      area[terra_pos(area_max_x, Y, X + 1)])
                 )
                 ) &&
                 area[terra_pos(area_max_x, Y + 1, X)] != '+' &&
                 area[terra_pos(area_max_x, Y - 1, X)] != '+' &&
                 area[terra_pos(area_max_x, Y, X + 1)] != '+' &&
                 area[terra_pos(area_max_x, Y, X - 1)] != '+'
                 ) {
                     area[terra_pos(area_max_x, Y, X)] = '+';

                     door_placed++;
                 }
    }

    *retsub = sub;
    return rc;
}

size_t terra_place_building_tiny(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x, subarea_t *retsub) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 9,
            retsub);
}

size_t terra_place_building_small(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x, subarea_t *retsub) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 12,
            retsub);
}

size_t terra_place_building_medium(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x, subarea_t *retsub) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 16,
            retsub);
}

size_t terra_place_building_big(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x, subarea_t *retsub) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 49,
            retsub);
}

size_t terra_place_building_large(char *area, size_t max_y, size_t max_x,
        size_t center_y, size_t center_x, subarea_t *retsub) {
    return terra_place_building(area, max_y, max_x, center_y, center_x, 64,
            retsub);
}

typedef size_t(*terra_building_func_t)(char*, size_t, size_t, size_t, size_t,
        subarea_t*);
static terra_building_func_t terra_building_func[CITY_SIZE] = {
    terra_place_building_tiny,
    terra_place_building_small,
    terra_place_building_medium,
    terra_place_building_big,
    terra_place_building_large
};

size_t terra_place_city(char *area, size_t area_max_y, size_t area_max_x,
        size_t center_y, size_t center_x, enum city_size size) {
    int rc = 0;

    // Place buildings

    // TODO adjust coefficients
    subarea_t sub;             // subarea for the building
    size_t R = size * 12 + 12; // fine selected random radius
    size_t N = size * 10 + 10; // number of buildings

    // city border limits
    size_t c_min_y = (R > center_y) ? 0 : center_y - R;
    size_t c_min_x = (R > center_x) ? 0 : center_x - R;
    size_t c_max_y = (R + center_y > area_max_y) ? area_max_y : R + center_y;
    size_t c_max_x = (R + center_x > area_max_x) ? area_max_x : R + center_x;

    size_t c_abs_min_y = area_max_y;
    size_t c_abs_min_x = area_max_x;
    size_t c_abs_max_y = 0;
    size_t c_abs_max_x = 0;

    for (size_t i = 0; i < N; i++) {
        size_t b_center_y = c_min_y + random() % (c_max_y - c_min_y + 1);
        size_t b_center_x = c_min_x + random() % (c_max_x - c_min_x + 1);

        rc += terra_building_func[random() % (size + 1)](
                area, area_max_y, area_max_x, b_center_y, b_center_x, &sub
                );

        if (sub.abs_min_y < c_abs_min_y) { 
            c_abs_min_y = sub.abs_min_y;
        }
        if (sub.abs_min_x < c_abs_min_x) { 
            c_abs_min_x = sub.abs_min_x;
        }
        if (sub.abs_max_y > c_abs_max_y) { 
            c_abs_max_y = sub.abs_max_y;
        }
        if (sub.abs_max_x > c_abs_max_x) { 
            c_abs_max_x = sub.abs_max_x;
        }
    }

    // Cleanup non-buildings (grass/forest/etc.)
    for (size_t i = c_abs_min_y; i <= c_abs_max_y; i++) {
        for (size_t j = c_abs_min_x; j <= c_abs_max_x; j++) {
            switch (area[terra_pos(area_max_x, i, j)]) {
                case '_':
                case '#':
                case '+':
                    continue;
                default:
                    area[terra_pos(area_max_x, i, j)] = 'x';
            }
        }
    }

    return rc;
}

int terra_connect(xy_t A, xy_t B, xy_t **path, size_t *pathlen) {
#define PATH (*path)
#define STEPS (*pathlen)

    PATH = NULL;
    STEPS = 0;
    xy_t curr = A;

    while (curr.x != B.x || curr.y != B.y) {
        if ((PATH = (xy_t*)realloc(PATH, (STEPS + 1) * sizeof(xy_t)))== NULL) {
            panic("Error allocating terra path!");
        }

        // TODO add randomness
        if (curr.x != B.x) {
            PATH[STEPS].x = (curr.x < B.x) ? curr.x + 1 : curr.x - 1;
        } else {
            PATH[STEPS].x = curr.x;
        }

        if (curr.y != B.y) {
            PATH[STEPS].y = (curr.y < B.y) ? curr.y + 1 : curr.y - 1;
        } else {
            PATH[STEPS].y = curr.y;
        }

        curr = PATH[STEPS++];

        // TODO add some kind of TTL
    }

    if (curr.x == B.x && curr.y == B.y) {
        return 1;
    }

#undef PATH
    return 0; // connection failed
}

size_t terra_place_roads(char *area, size_t area_max_y, size_t area_max_x, 
        xy_t A, xy_t B) {
    size_t rc = 0;

#define CURR (sub.lines[i].start[j])

    xy_t *path;
    size_t pathlen;

    if (terra_connect(A, B, &path, &pathlen) == 0) {
        fprintf(stderr, "%zu\n", pathlen);
        panic("Unable to connect A and B with road!");
    }

    for (size_t i = 0; i < pathlen; i++) {
        subarea_t sub = terra_subarea(area, area_max_y, area_max_x,
                path[i].y, path[i].x, 3, 3);

        for (size_t i = 0; i < sub.count; i++) {
            for (size_t j = 0; j < sub.lines[i].length; j++) {
                if (CURR != ' ' && CURR != '^') {
                    continue;
                }

                rc++; // we'll measure only new square for roads

                CURR = '%'; // TODO turn into grass
            }
        }
    }

#undef CURR

    return rc;
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
            if ((size_t)(random() % 101) <= density) {
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

    srandom(sysutime());

    // TODO fix sizeof(tile_t)
    if ((AREA = malloc(sizeof(char) * max_y * max_x)) == NULL) {
        panic("Error allocating terra!");
    }

    // First of all, initialize all with nothing
    for (size_t i = 0; i < max_y; i++) {
        for (size_t j = 0; j < max_x; j++) {
            AREA[i * max_x + j] = ' ';
        }
    }

    // Place forests
    size_t forest_square = 0;
    ttl = 64;

    if (forest < 10) {
        goto CITIES; // Forest? No, never heard of it.
    }

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

        // TODO revise an effect
        // size_y = size_y / 2 + random() % (1 + size_y / 2);
        // size_x = size_x / 2 + random() % (1 + size_x / 2);

        // TODO what's with density ?
        forest_square += terra_place_forest(
                terra_subarea(AREA, max_y, max_x,
                    center_y, center_x, size_y, size_x), // subarea
                density);                                // density
    }

    rc += forest_square;

CITIES:

    // Place cities
    rc += terra_place_city(AREA, max_y, max_x, 15, 15, CITY_MEDIUM);

    // TODO place roads between all the cities

    xy_t A = { 10, 20 };
    xy_t B = { 30, 90 };

    rc += terra_place_roads(AREA, max_y, max_x, A, B);

    // // Fill remaining with grass
    // for (int i = 0; i < max_y * max_x; i++) {
    //     if (AREA[i] == ' ') {
    //         AREA[i] = '.';
    //     }
    // }
#undef AREA

    return rc;
}

int main(int argc, char *argv[]) {   

    size_t max_y = 100;
    size_t max_x = 100;
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
