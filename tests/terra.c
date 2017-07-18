// vim: ts=4 sw=4 et :
/* Made by: KorG */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

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

void panic(char *str) {
    fprintf(stderr, "%s\n", str);
    exit(2);
}

void terra_visualize(char *area, size_t max_y, size_t max_x) {
    if (area == NULL) {
        return;
    }

    for (int i = 0; i < max_y; i++) {
        write(1, area + i * max_x, max_x);
        write(1, "\n", 1);
    }
}

int terra_create(char **area, size_t max_y, size_t max_x, city_t *cities,
        size_t cities_len, size_t forest) {
#define AREA (*area)

    int rc = 0;
    int ttl = 64; // number of tries

    srandomdev();
    srandom(random());

    // TODO fix sizeof(tile_t)
    if ((AREA = malloc(sizeof(char) * max_y * max_x)) == NULL) {
        panic("Error allocating terra!");
    }

    // First of all, initialize all with nothing
    for (int i = 0; i < max_y; i++) {
        for (int j = 0; j < max_x; j++) {
            AREA[i * max_x + j] = ' ';

            rc++;
        }
    }

    // Place trees based on random() % (forest + 1)
    for (int i = 0; i < max_y * max_x * forest / 100; i++) {
        int curr = random() % (max_y * max_x);

        if (AREA[curr] != ' ') {
            continue;
        }

        // currently all of forests have 33% density to make it possible to
        // walk through
        if (random() % 101 <= 33) {
            AREA[curr] = '^';
        }
    }

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

    // Fill remaining with grass
    for (int i = 0; i < max_y * max_x; i++) {
        if (AREA[i] == ' ') {
            AREA[i] = '.';
        }
    }
#undef AREA

    return 0;
}

int main(int argc, char *argv[]) {   

    size_t max_y = 24;
    size_t max_x = 80;
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

    return 0;
}
