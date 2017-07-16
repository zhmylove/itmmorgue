// vim: sw=4 ts=4 et :
#ifndef AREA_H
#define AREA_H

#include "stuff.h"
#include "tiles.h"
#include "levels.h"

void draw_area();
void area_init();
size_t lvltilepos(uint16_t max_x, uint16_t y, uint16_t x);
size_t tilepos(uint16_t y, uint16_t x);
void c_level_add(level_t *level);
void c_area_update(size_t ngroups, tileblock_t *tileblock);

#endif /* AREA_H */
