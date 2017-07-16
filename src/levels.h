// vim: sw=4 ts=4 et :
#ifndef LEVELS_H
#define LEVELS_H

#define MAX_LEVEL_NAME 8

typedef struct level {
    uint64_t id;
    char name[MAX_LEVEL_NAME];
    uint16_t max_y;
    uint16_t max_x;
    uint16_t size;
    tile_t *area;
} level_t;

#endif /* LEVELS_H */
