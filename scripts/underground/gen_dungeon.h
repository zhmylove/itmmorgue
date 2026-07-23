// vim: sw=4 ts=4 et :
#ifndef GEN_DUNGEON_H
#define GEN_DUNGEON_H

#include "gen_underground.h"
#include <sys/types.h>

#define OPPOSITE_DIR(d) ((d) ^ 2)

// 'adjusted' width and height if piece was pointing in this direction
#define ADJ_WIDTH(d, w, h) ((d)%2 > 0?(h):(w))
#define ADJ_HEIGHT(d, w, h) ((d)%2 > 0?(w):(h))
#define roll(a, b) (rand() % (b-a+1) + (a))

#define DG_ANY_COUNT -1

#define DG_PART_FLAGS_NONE 0
#define DG_PART_FLAGS_ENTRANCE 1

#define DG_FLAGS_NONE 0
#define DG_FLAGS_BUILT 1

typedef enum { GEN_CORRIDOR, GEN_ROOM } piece_class_t;
typedef enum { GEN_STATIC, GEN_DYNAMIC} piece_gen_t;
typedef enum { NORTH, EAST, SOUTH, WEST, UP, DOWN} dir_t;

static const int dir_offsets[18] = {
     0, -1,  0, // NORTH
     1,  0,  0, // EAST
     0,  1,  0, // SOUTH
    -1,  0,  0, // WEST
     0,  0, -1, // UP
     0,  0,  1  // DOWN
};

struct dg_list;
struct dg_piece;
struct dg_gen_part;
struct dg_parts_array;

typedef struct dg_gen_part* (*part_sel_fptr) (struct dg_parts_array*);
typedef struct dg_piece* (*piece_gen_fptr) (struct level*, struct dg_gen_part*, struct coords, dir_t, struct dg_list* );
typedef void (*piece_build_fptr) (struct level*, struct dg_piece* , struct dg_list*, struct dg_list*, struct dg_parts_array*);

// Generation info for specific dungeon part
typedef struct dg_gen_part {
    int width, height, depth;     // Dimensions
    char ** data;                 // Part map: c-strings of rows
    piece_class_t class;          // Part class: room or corridor
    piece_gen_t gen_type;         // Generation type: static or dynamic
    unsigned int weight;          // Generation weight
    struct coords * conns;        // Array of possible connection points
    size_t max_conns;             // Length of conns array
    ssize_t count, max_count;     // Maximum allowed number
    piece_gen_fptr gen_fptr;      // Function ptrs for dynamic gen:
    piece_build_fptr build_fptr;  //  * gen_fptr returns new dungeon piece
    int flags;                    //  * build_fptr actually builds that piece
} dg_gen_part_t;                  //    on the level and queues more pieces

// Array of dungeon generation parts
typedef struct dg_parts_array {
    struct dg_gen_part **data;
    size_t length;
    int total_weight;
} dg_parts_array_t;

// Dungeon piece located on the level
typedef struct dg_piece {
    struct coords pos;          // 3d position
    int width, height, depth;   // Dimensions
    dir_t dir;                  // Orientation
    struct dg_gen_part *type;   // Dungeon part type
    int flags;
    struct coords anchor;       // anchor tile used to connect
                                // to parent dungeon piece
} dg_piece_t;

// List of dungeon pieces
typedef struct dg_list {
    struct dg_list_node *first;
    struct dg_list_node *last;
    size_t count;
} dg_list_t;

typedef struct dg_list_node {
    void* who;
    struct dg_list_node *next;
} dg_list_node_t;

void gen_dungeon(level_t *l, coords_t *stairs, size_t stairs_count,
        int max_rooms, int max_pieces);

dg_piece_t* select_piece(level_t *l, coords_t pos, dir_t dir,
        dg_list_t *pieces, dg_parts_array_t *parts);
void build_piece(level_t *l, dg_piece_t *p, dg_list_t *pieces, dg_list_t *bo,
        dg_parts_array_t *parts);
void queue_piece(level_t *l, coords_t pos, dir_t d, dg_piece_t *parent,
        dg_list_t *pieces, dg_list_t *bo, dg_parts_array_t *parts);

dg_piece_t* create_piece(coords_t pos, int w, int h, int d, dir_t dir,
        dg_gen_part_t *t, coords_t anchor);

dg_piece_t* point_occupied(level_t *l, coords_t pos, dg_list_t *pieces);
dg_piece_t* intersected_piece(level_t *l, coords_t a, coords_t b,
        dg_list_t *pieces);

dg_gen_part_t* weighted_part_roll(dg_parts_array_t* parts);

coords_t sm2l(coords_t pos, int w, int h, int d, dir_t dir);
coords_t psm2l(dg_piece_t *p, coords_t pos);
dir_t get_conn_dir(dg_piece_t *p, int conn_id);
int doorway_scan(level_t* l, coords_t p, dir_t d);
void closeoff_connection(level_t *l, coords_t p, dir_t d);
void openup_connection(level_t *l, coords_t p, dir_t d, char door);

void add_gen_part(dg_parts_array_t* arr, dg_gen_part_t* p);
dg_gen_part_t* load_static_gen_part(const char *filename);
dg_parts_array_t load_gen_parts();

void die(const char* reason);

#endif /* GEN_DUNGEON_H */
