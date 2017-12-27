// vim: sw=4 ts=4 et :
#ifndef LEVELS_H
#define LEVELS_H

#define MAX_LEVEL_NAME 32

#define WIDTH 256
#define HEIGHT 64

#define GEN_SH "scripts/Gen.sh -w%d -h%d"
#define GEN_MAX 128

typedef struct level {
    uint64_t id;
    uint16_t max_y;
    uint16_t max_x;
    uint32_t size;
    tile_t *area;
    char name[MAX_LEVEL_NAME];
} level_t;

void s_levels_init();
void s_level_send(size_t level, player_t *player);
void s_area_send(size_t level, player_t *player);

#endif /* LEVELS_H */
