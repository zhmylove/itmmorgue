// vim: et sw=4 ts=4 :
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gen_underground.h"
#include <dirent.h>
#include "gen_dungeon.h"
#include <string.h>
#include <limits.h>

#include "gen_dungeon_parts.h"

#define PIECES_DIR "./pieces"

// Generated room counter
static int room_count = 0;

void die(const char* reason) {
    fprintf(stderr, "%s\n", reason);
    exit(EXIT_FAILURE);
}

/*
 * Allocates and initializes dungeon piece object (dg_piece_t).
 *
 * pos : top-left dungeon piece position
 * w : dungeon piece width
 * h : dungeon piece height
 * d : dungeon piece depth
 * dir : dungeon piece direction
 * t : dungeon piece type
 * anchor : level coords of its anchor (tile used to connect to other piece)
 *
 * ret : ptr to initialized dungeon piece object
 */
dg_piece_t* create_piece(coords_t pos, int w, int h, int d, dir_t dir,
         dg_gen_part_t *t, coords_t anchor) {
    dg_piece_t *r = malloc(sizeof(dg_piece_t));
    r->pos = pos;
    r->width = ADJ_WIDTH(dir, w, h);
    r->height = ADJ_HEIGHT(dir, w, h);
    r->depth = d;
    r->dir = dir;
    r->type = t;
    r->anchor = anchor;
    r->flags = DG_FLAGS_NONE;
    return r;
}

dir_t rotate_dir(dir_t a, dir_t b) {
    if (a == UP || a == DOWN) return a;
    return (a + b) % 4;
}

/*
 * Performs simple sanity checks for passed dungeon generation part.
 */
static void gen_part_sanity_check(dg_gen_part_t *p) {
    if (p->gen_type == GEN_STATIC && (p->gen_fptr || p->build_fptr)) {
        die("static part has non-NULL generation fptrs");
    }
    if (p->gen_type == GEN_DYNAMIC && (!p->gen_fptr || !p->build_fptr)) {
        die("dynamic part has NULL generation fptrs");
    }
    if (p->class > GEN_ROOM) {
        die("invalid class type");
    }

    if (p->gen_type == GEN_STATIC) {
        for (int i = 0 ; i < p->height; ++i) {
            if ((int)strlen(p->data[i]) != p->width) {
                die("internal map width does not match specified width");
            }
        }

        char flag = 0;
        for (size_t i = 0; i < p->max_conns; ++i) {
            if (p->conns[i].y == p->height-1) flag = 1;
        }
        if (!flag) {
            die("no connection at 'southern' side");
        }
    }
}

/*
 * Adds dungeon generation part to an array of such parts.
 */
void add_gen_part(dg_parts_array_t *arr, dg_gen_part_t *p) {
    gen_part_sanity_check(p);

    arr->data = realloc(arr->data, sizeof(dg_gen_part_t*) * (arr->length + 1));
    arr->data[arr->length] = p;
    arr->length++;
    arr->total_weight += p->weight;

    /* should be somewhere else */
    p->count = 0;
}

/*
 * Very simple dungeon generation part loader.
 *
 * filename : path to file containing generation info for specific
 *            dungeon piece
 *
 * ret : ptr to loaded dungeon generation part
 */
dg_gen_part_t* load_static_gen_part(const char *filename) {
    fprintf(stderr, "loading part \"%s\"\n", filename);
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "failed to open it.\n");
        return NULL;
    }

    int smi = 0;
    dg_gen_part_t *r = malloc(sizeof(dg_gen_part_t));
    memset(r, 0, sizeof(struct dg_gen_part));

    r->gen_type = GEN_STATIC;
    r->gen_fptr = NULL;
    r->build_fptr = NULL;

    if (fscanf(f, "%d%d%d%d%d%zd%zu", &r->width, &r->height, &r->depth,
            (int*)&r->class, &r->weight, &r->max_count, &r->max_conns) != 7) {
        goto rip;
    }

    /* Internal map and defined connection points */
    r->data = malloc(sizeof(char*) * r->height * r->depth);
    r->conns = malloc(sizeof(coords_t) * r->max_conns);
    for (size_t i = 0; i < r->max_conns; ++i) {
        if (fscanf(f, "%d%d%d", &r->conns[i].x, &r->conns[i].y,
                &r->conns[i].z) != 3) {
            goto rip_conns;
        }
    }
    fgetc(f);
    for (int i = 0; i < r->depth; ++i) {
        for (smi = 0; smi < r->height; ++smi) {
            r->data[smi + i*r->height] = malloc(r->width + 1);
            int len = fread(r->data[smi + i*r->height], 1, r->width + 1, f);
            if (len != r->width + 1) goto rip_submap;
            (r->data[smi + i*r->height])[r->width] = '\0';
        }
    }

    r->flags = DG_PART_FLAGS_NONE;

    return r;

rip_submap:
    for (int j = 0; j < r->depth; ++j) {
        for (int i = 0; i < MIN(smi, r->height); ++i) {
            free(r->data[i + j * r->height]);
        }
    }
rip_conns:
    free(r->conns);
    free(r->data);
rip:
    free(r);
    return NULL;
}

/*
 * Returns an array of available dungeon generation parts.
 */
dg_parts_array_t load_gen_parts() {
    dg_parts_array_t result = (dg_parts_array_t) {
        .data = NULL,
        .length = 0,
        .total_weight = 0
    };

    // First, hardcoded parts are added, such as dynamically generated parts.
    add_hardcoded_dungeon_parts(&result);

    // Then attempt to load more dungeon generation parts from designated
    // PIECES_DIR directory
    DIR *d;
    struct dirent *dir;
    if ((d = opendir(PIECES_DIR))) {
        while ((dir = readdir(d))) {
            int l = strlen(dir->d_name);

            // Check for files ending with '.idp'
            if (l > 4 && strncmp(dir->d_name + l - 4, ".idp", 4) == 0) {
                char name[sizeof(PIECES_DIR) + 256];

                // Form path for found files relative to PIECES_DIR
                strncpy(name, PIECES_DIR, sizeof(PIECES_DIR));
                name[sizeof(PIECES_DIR)-1] = '/';
                strncpy(name + sizeof(PIECES_DIR), dir->d_name, l+1);

                struct dg_gen_part *p = load_static_gen_part(name);
                if (p) {
                    fprintf(stderr, "successfully loaded: %dx%dx%d\n",
                        p->width, p->height, p->depth);
                    add_gen_part(&result, p);
                }
            }
        }
        closedir(d);
    }
    else {
        fprintf(stderr, "failed to open piece directory %s\n", PIECES_DIR);
    }

    return result;
}

void append_list(void *p, dg_list_t *l) {
    dg_list_node_t *nn = malloc(sizeof(dg_list_node_t));
    nn->next = NULL;
    nn->who = p;

    if (l->last) l->last->next = nn;
    else l->first = nn;

    l->last = nn;
    l->count++;
}

void delete_list_nodes(dg_list_node_t *n) {
    if (n == NULL) return;

    delete_list_nodes(n->next);
    free(n);
}

void flush_list(dg_list_t *l) {
    delete_list_nodes(l->first);
    
    l->first = NULL;
    l->last = NULL;
    l->count = 0;
}

/*
 * Converts dungeon part internal map coords into level coords.
 *
 * pos : internal map coordinates
 * w : dungeon part width
 * h : dungeon part height
 * d : dungeon part depth
 * dir : dungeon part direction
 *
 * ret : 'pos' coordinates projected onto level
 */
coords_t sm2l(coords_t pos, int w, int h, int d, dir_t dir) {
    (void)d;
    switch(dir) {
        case EAST:  return (coords_t) { w-1 - pos.y, pos.x, pos.z };
        case SOUTH: return (coords_t) { w-1 - pos.x, h-1 - pos.y, pos.z };
        case WEST:  return (coords_t) { pos.y, h-1 - pos.x, pos.z };
        case NORTH:
        default: return (coords_t) { pos.x, pos.y, pos.z };
    }
}

coords_t psm2l(dg_piece_t *p, coords_t pos) {
    return sm2l(pos, p->width, p->height, p->depth, p->dir);
}

/*
 * Returns dungeon piece occupying this tile.
 */
dg_piece_t* point_occupied(level_t *l, coords_t pos, dg_list_t *pieces) {
    (void)pieces;
    if(!VALID_COORDS(l, pos)) return NULL;
    return (dg_piece_t*) ROOM_AT(l, pos.x, pos.y, pos.z);
}

/*
 * Returns any dungeon piece that intersects this 3d area.
 *
 * l : ptr to dungeon level object
 * a : top-left area cuboid vertice coordinates
 * b : bottom-right area cuboid vertice coordinates
 *
 * ret : ptr to any dungeon piece in cuboid defined by a and b or NULL if none
 *       present
 */
dg_piece_t* intersected_piece(level_t *l, coords_t a, coords_t b, 
        dg_list_t *pieces) {
    (void)pieces;
    if (!VALID_COORDS(l, a) || !VALID_COORDS(l, b)) return NULL;

    for (int k = a.z; k <= b.z; ++k)
        for (int j = a.y; j < b.y; ++j)
            for (int i = a.x; i < b.x; ++i)
                if (ROOM_AT(l, i, j, k) != NULL) {
                    return (dg_piece_t*)ROOM_AT(l, i, j, k);
                }
    return NULL;
}

/*
 * Updates total weight of all dungeon generation parts in array.
 * Does not take into account parts that can't be generated (e.g. exceeded max
 * allowed count)
 */
void recalculate_parts_total_weight(dg_parts_array_t* a) {
    a->total_weight = 0;

    for (size_t i = 0; i < a->length; ++i) {
        dg_gen_part_t *p = a->data[i];

        if (p->max_count == DG_ANY_COUNT || p->count < p->max_count) {
            a->total_weight += p->weight;
        }
    }
}

/*
 * Randomly rolls for a dungeon part in supplied array using their weights.
 */
dg_gen_part_t* weighted_part_roll(dg_parts_array_t *parts) {
    if (parts == NULL) return NULL;

    unsigned int roll = rand() % parts->total_weight;

    for (size_t i = 0; i < parts->length; ++i) {
        struct dg_gen_part *p = parts->data[i];

        // If there is enough of this dungeon part, do not bother.
        if (p->max_count != DG_ANY_COUNT && p->count >= p->max_count) continue;

        if (p->weight > roll) {
            if (p->max_count != DG_ANY_COUNT && ++p->count >= p->max_count) {
                recalculate_parts_total_weight(parts);
            }
            return parts->data[i];
        }
        roll -= parts->data[i]->weight;
    }

    // Should not be possible if total weight is accurate.
    return NULL;
}

/*
 * Projects dungeon piece onto the dungeon level. Does not update actual level
 * map.
 */
static void project_piece(level_t *l, dg_piece_t *p, dg_list_t *pieces) {
    for (int k = 0; k < p->depth; ++k)
        for (int j = 0; j < p->height; ++j)
            for (int i = 0; i < p->width; ++i)
                ROOM_AT(l, p->pos.x+i, p->pos.y+j, p->pos.z+k) = p;

    append_list(p, pieces);
}

/*
 * Returns the direction dungeon piece connection is pointing to.
 *
 * t : dungeon part type of that dungeon piece
 * dir : dungeon piece orientation
 * conn : id of dungeon part connection used
 *
 * ret : resulting direction
 */
static dir_t _get_conn_dir(dg_gen_part_t *t, dir_t dir, int conn) {
    int d;
    if (t->conns[conn].x == 0) d = WEST;
    else if (t->conns[conn].x == t->width-1) d = EAST;
    else if (t->conns[conn].y == 0) d = NORTH;
    else if (t->conns[conn].y == t->height-1) d = SOUTH;
    else if (t->conns[conn].z == 0) d = UP;
    else if (t->conns[conn].z == t->depth-1) d = DOWN;

    return rotate_dir(d, dir);
}


dir_t get_conn_dir(dg_piece_t *p, int conn_id) {
    return _get_conn_dir(p->type, p->dir, conn_id);
}

/*
 * Checks if passed dungeon generation part has any upward connections.
 */
static int has_upward_connection(dg_gen_part_t *t)
{
    for (size_t i = 0; i < t->max_conns; ++i) {
        dir_t d = _get_conn_dir(t, NORTH, i);
        if (d == UP) return 1;
    }
    return 0;
}

/*
 * Selects parts that have upward connections from another parts array.
 */
dg_parts_array_t load_starting_parts(dg_parts_array_t from) {
    dg_parts_array_t result = (dg_parts_array_t) {
        .data = NULL,
        .length = 0,
        .total_weight = 0
    };

    for (size_t i = 0; i < from.length; ++i) {
        if (from.data[i]->gen_type == GEN_STATIC) {
            if (has_upward_connection(from.data[i])) {
                add_gen_part(&result, from.data[i]);
            }
        }
        else if (from.data[i]->flags & DG_PART_FLAGS_ENTRANCE) {
            add_gen_part(&result, from.data[i]);
        }
    }

    fprintf(stderr, "%zd entrance-capable parts.\n", result.length);
    return result;
}

/*
 * Attempts to randomly select a dungeon piece that will fit into the level.
 *
 * l : ptr to dungeon level object
 * pos : level coords of tile to be used as connection point
 * dir : orientation of new dungeon piece
 * pieces : list of already present dungeon pieces
 * parts : array of available dungeon generation parts
 *
 * ret : ptr to dungeon piece object on success, otherwise NULL
 */
dg_piece_t* select_piece(level_t *l, coords_t pos, dir_t dir, dg_list_t *pieces,
        dg_parts_array_t* parts) {
    int c;
    dg_gen_part_t *sp = weighted_part_roll(parts);

    // weighted_part_roll() can fail if all parts are finite.
    if (sp == NULL) return NULL;

    if (sp->gen_type == GEN_STATIC) {
        // In case of orthogonal direction
        if (dir < 4) {
            // Since those internal maps are oriented north,
            // any connection at the 'southern' side is picked.
            do {
                c = rand() % sp->max_conns;
            } while (sp->conns[c].y != sp->height-1);
        }
        // up & down
        else {
            // not implemented
            if (dir == DOWN) return NULL;

            if (!has_upward_connection(sp)) return NULL;

            do {
                c = rand() % sp->max_conns;
            } while (_get_conn_dir(sp, NORTH, c) != dir);

            // As of now, dungeon piece can't be oriented on the z-axis.
            // Instead we pick any random orthogonal direction.
            dir = rand() % 4;
        }

        // Piece's top-left tile coords on the level map.
        coords_t cc = sm2l(sp->conns[c],
            ADJ_WIDTH(dir, sp->width, sp->height),
            ADJ_HEIGHT(dir, sp->width, sp->height), sp->depth, dir);

        pos.x -= cc.x;
        pos.y -= cc.y;
        pos.z -= cc.z;

        // and bottom-right tile coords
        coords_t pos2 = (struct coords) {
            .x = pos.x + ADJ_WIDTH(dir, sp->width, sp->height) - 1,
            .y = pos.y + ADJ_HEIGHT(dir, sp->width, sp->height) - 1,
            .z = pos.z + sp->depth - 1};

        if ( !VALID_COORDS(l, pos) || !VALID_COORDS(l, pos2)) {
            return NULL;
        }

        if (intersected_piece(l, pos, pos2, pieces) == NULL) {
            return create_piece(pos, sp->width, sp->height, sp->depth, dir, sp,
                    (coords_t){ pos.x + cc.x, pos.y + cc.y, pos.z + cc.z});
        }

        return NULL;
    } else {
        return sp->gen_fptr(l, sp, pos, dir, pieces);
    }
}

/*
 * Checks if tile is part of a doorway.
 */
int doorway_scan(level_t* l, coords_t p, dir_t d) {
    if (!VALID_COORDS(l, p)) return 0;

    if (AT(l, p.x, p.y, p.z) == '+') {
        switch(d) {
            case EAST:
            case WEST:
                if (p.y > 0 && AT(l, p.x, p.y-1, p.z) != '#') return 0;
                if (p.y < (l->h-1) && AT(l, p.x, p.y+1, p.z) != '#') return 0;
                return 1;
            case NORTH:
            case SOUTH:
                if (p.x > 0 && AT(l, p.x-1, p.y, p.z) != '#') return 0;
                if (p.x < (l->w-1) && AT(l, p.x+1, p.y, p.z) != '#') return 0;
                return 1;
            default:
                return 0;
        }
    }
    else if (AT(l, p.x, p.y, p.z) == '#') {
        switch(d) {
            case EAST:
            case WEST:
                if (p.y > 0 && AT(l, p.x, p.y-1, p.z) == '+') return 1;
                if (p.y < (l->h-1) && AT(l, p.x, p.y+1, p.z) == '+') return 1;
                return 0;
            case NORTH:
            case SOUTH:
                if (p.x > 0 && AT(l, p.x-1, p.y, p.z) == '+') return 1;
                if (p.x < (l->w-1) && AT(l, p.x+1, p.y, p.z) == '+') return 1;
                return 0;
            default:
                return 0;
        }
    }
    return 0;
}

/*
 * Properly (?) walls off dungeon piece connection point.
 */
void closeoff_connection(level_t *l, coords_t p, dir_t d) {
    switch(d) {
        case NORTH:
        case SOUTH:
            if (p.x > 0 && AT(l, p.x-1, p.y, p.z) == ' ')
                AT(l, p.x, p.y, p.z) = '#';

            AT(l, p.x, p.y, p.z) = '#';

            if (p.x < l->w-1 && AT(l, p.x+1, p.y, p.z) == ' ')
                AT(l, p.x, p.y, p.z) = '#';
            break;
        case EAST:
        case WEST:
            if (p.y > 0 && AT(l, p.x, p.y-1, p.z) == ' ')
                AT(l, p.x, p.y, p.z) = '#';

            AT(l, p.x, p.y, p.z) = '#';

            if (p.y < l->h-1 && AT(l, p.x, p.y+1, p.z) == ' ')
                AT(l, p.x, p.y, p.z) = '#';
            break;
        default:
            AT(l, p.x, p.y, p.z) = '#';
            break;
    }
}

/* Opens up dungeon piece connection point.
 *
 * l : ptr to dungeon level object
 * p : level coords of connection point tile
 * d : connection point direction
 * door : non-zero if a door should be installed instead of usual floor
 *
 * ret : nothing
 */
void openup_connection(level_t *l, coords_t p, dir_t d, char door) {
    if (!is_floor(AT(l, p.x, p.y, p.z)))
        AT(l, p.x, p.y, p.z) = door ? '+' : '.';

    switch(d) {
        case NORTH:
        case SOUTH:
            if (p.x > 0 && AT(l, p.x-1, p.y, p.z) == ' ')
                AT(l, p.x, p.y, p.z) = '#';
            if (p.x < l->w-1 && AT(l, p.x+1, p.y, p.z) == ' ')
                AT(l, p.x, p.y, p.z) = '#';
            break;
        case EAST:
        case WEST:
            if (p.y > 0 && AT(l, p.x, p.y-1, p.z) == ' ')
                AT(l, p.x, p.y, p.z) = '#';
            if (p.y < l->h-1 && AT(l, p.x, p.y+1, p.z) == ' ')
                AT(l, p.x, p.y, p.z) = '#';
            break;
        case UP:
            AT(l, p.x, p.y, p.z) = '<';
            break;
        case DOWN:
            AT(l, p.x, p.y, p.z) = '>';
            break;
    }
}

/*
 * Attempts to queue a new dungeon piece for construction from specified
 * position.
 *
 * l : ptr to dungeon level object
 * pos : specified level position
 * d : required dungeon piece orientation
 * parent : ptr to the dungeon piece that new piece will be connected to
 * pieces : list of already-present dungeon pieces
 * bo : list of dungeon pieces to be built
 * parts : array of available dungeon parts
 *
 * ret : nothing
 */
void queue_piece(level_t *l, coords_t pos, dir_t d, dg_piece_t *parent,
        dg_list_t *pieces, dg_list_t *bo, dg_parts_array_t *parts) {

    dg_gen_part_t *t = parent->type;
    // Anchor coordinates of queued piece.
    coords_t anchor = (coords_t) {
        pos.x + dir_offsets[3*d],
        pos.y + dir_offsets[3*d+1],
        pos.z + dir_offsets[3*d+2]
    };

    if (!VALID_COORDS(l, anchor)) {
        closeoff_connection(l, pos, d);
        return;
    }

    dg_piece_t *o = point_occupied(l, anchor, pieces);

    // If there is no piece right beyond the possible connection, we try to
    // build one.
    if (o == NULL) {
        dg_piece_t *next = select_piece(l, anchor, d, pieces, parts);
        // If suitable dungeon part is found, it is queued unless build order
        // list is not passed
        if (next && bo && parts) {
            // Only rooms can have doors.
            // If corridors had them, it would be extremely annoying to open
            // them every 2-5 tiles
            if (t->class != GEN_CORRIDOR) {
                openup_connection(l, pos, d, '+');
            }
            else {
                openup_connection(l, pos, d, 0);
            }
            next->dir = d;

            project_piece(l, next, pieces);
            append_list(next, bo);
            if (next->type->class == GEN_ROOM) room_count++;
        } else {
            // Otherwise the connection point is closed off
            closeoff_connection(l, pos, d);
        }
    } else {
        // There is a piece, we will try to open up a passage
        // to make the dungeon more varied

        // Can't edit dungeon pieces that are not built yet
        if (!(o->flags & DG_FLAGS_BUILT)) return;

        // If the occupied tile is in corner of that piece, then do nothing.
        // Such passages would look very ugly in case of rectangular rooms
        if ((anchor.x == o->pos.x || anchor.x == o->pos.x + o->width-1) &&
            (anchor.y == o->pos.y || anchor.y == o->pos.y + o->height-1) &&
            (anchor.z == o->pos.z || anchor.z == o->pos.z + o->depth-1)) {
            closeoff_connection(l, pos, d);
            return;
        }

        // If it is a wall (e.g. it is not an already existing passage),
        // check if there is open space beyond it.
        if (AT(l, anchor.x, anchor.y, anchor.z) == '#') {
            // The tile beyond the wall
            coords_t pt = (coords_t) {
                anchor.x + dir_offsets[3*d],
                anchor.y + dir_offsets[3*d+1],
                anchor.z + dir_offsets[3*d+2]
            };

            if (!VALID_COORDS(l, pt) || !is_floor(AT(l, pt.x, pt.y, pt.z))) {
                return;
            }

            // There is open space, if it is not a part of some doorway
            // then we can finally open a new passage.
            if (!doorway_scan(l, anchor, d)) {
                AT(l, pos.x, pos.y, pos.z) = '.';
                AT(l, anchor.x, anchor.y, anchor.z) = '.';
            }
        } else if(!is_floor(AT(l, anchor.x, anchor.y, anchor.z))) {
            closeoff_connection(l, pos, d);
        }
    }
}

/*
 * Builds specified dungeon piece on the level. Also queues new dungeon pieces
 * connected to it.
 *
 * l : ptr to dungeon level object
 * p : ptr to the dungeon piece
 * pieces : list of already-present dungeon pieces
 * bo : list of dungeon pieces to be built
 * parts : list of available dungeon parts
 *
 * ret : nothing
 */
void build_piece(level_t *l, dg_piece_t *p, dg_list_t *pieces, dg_list_t *bo,
        dg_parts_array_t* parts) {

    static int count = 0;   // counter for built pieces
    size_t doff = 0;        // depth offset for internal map selection
    dg_gen_part_t *t = p->type;
    char ** bp = p->type->data;

    count++;

    if (t->gen_type == GEN_STATIC) {
        for (int k = 0; k < p->depth; ++k) {
            doff = p->type->height * k;
            for (int j = 0; j < p->height; ++j) {
                for (int i = 0; i < p->width; ++i) {
                    switch(p->dir) {
                        case NORTH:
                            // Does not fit into 80 chars sadly
                            AT(l, p->pos.x+i, p->pos.y+j, p->pos.z+k)
                                = bp[doff + j][i];
                            break;
                        case EAST:
                            AT(l, p->pos.x+i, p->pos.y+j, p->pos.z+k)
                                = bp[doff + p->width-1 - i][j];
                            break;
                        case SOUTH:
                            AT(l, p->pos.x+i, p->pos.y+j, p->pos.z+k)
                                = bp[doff + p->height-1 - j][p->width-1 - i];
                            break;
                        case WEST:
                            AT(l, p->pos.x+i, p->pos.y+j, p->pos.z+k)
                                = bp[doff + i][p->height-1 - j];
                            break;
                        default:
                            fprintf(stderr, "bad piece direction %d\n", p->dir);
                            exit(EXIT_FAILURE);
                            break;
                    }
                }
            }
        }

        // If anchor tile is not on the sides, then it is a staircase, not an
        // ordinary passage.
        if (p->anchor.x != p->pos.x && p->anchor.y != p->pos.y &&
            p->anchor.x != p->pos.x+p->width-1 && p->anchor.y != p->pos.y+p->height-1) {
            AT(l, p->anchor.x, p->anchor.y, p->anchor.z) = '<';
        } else {
            AT(l, p->anchor.x, p->anchor.y, p->anchor.z) = '.';
        }

        // Try to queue up more dungeon pieces for every possible connection.
        // If it's not possible queue_piece() will close them off.
        for (size_t i = 0; i < t->max_conns; ++i) {
            dir_t d = get_conn_dir(p, i);

            // converting connection internal piece coords into level coords
            coords_t lc = psm2l(p, t->conns[i]);
            lc.x += p->pos.x;
            lc.y += p->pos.y;
            lc.z += p->pos.z;

            queue_piece(l, lc, d, p, pieces, bo, parts);
        }
    } else t->build_fptr(l, p, pieces, bo, parts);
    p->flags |= DG_FLAGS_BUILT;
}

/*
 * Generates dungeon using level data and array of upstairs level positions.
 *
 * l : level info & data
 * stairs : array of stairs coords
 * stairs_count : length of stairs array
 * max_rooms : maximum number of rooms
 * max_pieces : maximum number of dungeon pieces
 *
 * ret : nothing
 */
void gen_dungeon(level_t *l, coords_t *stairs, size_t stairs_count,
        int max_rooms, int max_pieces) {
    dg_list_t pieces =  (dg_list_t) { NULL, NULL, 0 };
    dg_list_t build_order = (dg_list_t) { NULL, NULL, 0 };
    dg_list_t next_bo = (dg_list_t) { NULL, NULL, 0 };
    dg_parts_array_t parts = load_gen_parts();
    // Dungeon parts that can have an upward staircase.
    dg_parts_array_t entrance_parts = load_starting_parts(parts);

    dg_piece_t *start;

    // Select all starting pieces for construction.
    for (size_t i = 0; i < stairs_count; ++i) {
        int tries = 0;
        do {
            start = select_piece(l, stairs[i], UP, &pieces, &entrance_parts);

            // With extremely small maps or very close stairs positions
            // the algorithm will fail to start.
        } while (start == NULL && tries++ < 1000);

        if (start == NULL) {
            fprintf(stderr, "failed to build a starting piece with upstairs at"
                "(%d, %d, %d) after 1000 tries.\n", stairs[i].x, stairs[i].y,
                stairs[i].z);
            exit(EXIT_FAILURE);
        }

        project_piece(l, start, &pieces);
        append_list(start, &build_order);
    }

    // Algorithm main loop: queue, and then build pieces
    // until the piece limit is hit or it's not possible to build any more
    // dungeon pieces (the build order list is empty).
    while (pieces.count < (size_t)max_pieces && room_count < max_rooms
        && build_order.count > 0) {
        for (dg_list_node_t* i = build_order.first; i; i = i->next) {
            build_piece(l, (dg_piece_t*)i->who, &pieces, &next_bo, &parts);
        }

        flush_list(&build_order);
        build_order = next_bo;
        next_bo = (dg_list_t) { NULL, NULL, 0 };
    }

    // Build leftover dungeon pieces without queueing up more.
    for (dg_list_node_t *i = build_order.first; i; i = i->next) {
        build_piece(l, (dg_piece_t*)i->who, &pieces, NULL, NULL);
    }
    fprintf(stderr, "%zd pieces\n", pieces.count);
}

int main(int argc, char* argv[]) {
    int pc, rc;

    if (argc < 5) {
        fprintf(stderr, "usage: %s [max pieces] [max rooms] "
            "[[stairs.x] [stairs.y] ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    pc = atoi(argv[1]); // max dungeon pieces
    rc = atoi(argv[2]); // max rooms

    if ((argc - 3) % 2 != 0) {
        fprintf(stderr, "mismatching starting piece coords\n");
        return EXIT_FAILURE;
    }

    int spcc = (argc - 3) / 2;  // number of stairs positions passed
    coords_t *spc = malloc(spcc * sizeof(coords_t));

    for (int i = 0; i < spcc; ++i) {
        spc[i].x = atoi(argv[3 + 2*i]);
        spc[i].y = atoi(argv[3 + 2*i + 1]);
        spc[i].z = 0;   // start at the lowest depth
    }

    level_t *l = read_level(0);
    srand(time(NULL));

    room_count = 0;

    gen_dungeon(l, spc, spcc, rc, pc);

    write_level(l, 1);

    return EXIT_SUCCESS;
}
