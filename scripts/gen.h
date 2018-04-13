#ifndef GEN_H
#define GEN_H

#include "array.h"

#define DEFAULT (-1)
#define rand_double(x) ((double)rand() / RAND_MAX * (x))

int level(int lvl);
void free_regex(char* regex);
array_t** _get_world_ref(void);
array_t* get_level_ref(int lvl);
void print_level(int lvl);
array_t* read_level(int lvl);
psize_t get_size(int lvl);

void array_rotate(array_t *array, int direction);

void overlay_somehow(array_t *array, int rotate, int overlay);
void overlay_anywhere(array_t *array, int padding, char pchar);
void overlay_unsafe(int y, int x,  array_t *building);
void recreate_level_unsafe(int y, int x, char pchar);
psize_t get_free_area(int h, int w, int p, char pchar);
char check_area_is_free(int y, int x, int h, int w);
void _fill_area_with_char(int y, int x, int h, int w, char pchar);
array_t* generate_blurred_area(int level, char pchar, double factor);


#endif /* GEN_H */