// vim: sw=4 ts=4 et :
#ifndef GEN_DUNGEON_PARTS_H
#define GEN_DUNGEON_PARTS_H

#include "gen_underground.h"
#include "gen_dungeon.h"

void add_hardcoded_dungeon_parts(dg_parts_array_t *a);

dg_piece_t* simple_room_gen(level_t* l, dg_gen_part_t *p, coords_t a, dir_t dir, dg_list_t *pieces);
void simple_room_build(level_t* l, dg_piece_t* p, dg_list_t *pieces, dg_list_t *build_order, dg_parts_array_t *parts);
void column_room_build(level_t* l, dg_piece_t* p, dg_list_t *pieces, dg_list_t *build_order, dg_parts_array_t *parts);

#endif /* GEN_DUNGEON_PARTS_H */
