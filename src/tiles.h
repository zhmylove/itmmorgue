// vim: sw=4 ts=4 et :
#ifndef TILES_H
#define TILES_H

typedef struct tile {
    enum stuff top;             // type of the tile
    enum colors color;          // server-specified attributes
    uint16_t y;                 // absolute Y
    uint16_t x;                 // absolute X
    struct tile *underlying;    // pointer to underlying tile
} tile_t;

typedef struct tileblock {
    uint32_t count;
    uint32_t zcount;
    tile_t tiles[1];
} tileblock_t;

#endif /* TILES_H */
