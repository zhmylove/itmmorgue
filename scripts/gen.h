#ifndef GEN_H
#define GEN_H

#define rand_double(x) ((double)rand() / RAND_MAX * x)

extern int SIZE;

typedef struct size {
	int 	height;
	int 	width;
} psize_t;

typedef struct array {
	char** 			array;
	psize_t 	size;
} array_t;


array_t* _get_world_ref(void);
array_t* get_level_ref(int lvl);
void print_level(int lvl);
array_t* read_level(int lvl);
psize_t get_size(int lvl);

void _reverse(array_t *arr);
void _flip(array_t *arr);
void _cw(array_t *arr);
void array_rotate(array_t *array, int direction);

void print_array(array_t *arr);
array_t copy_array(char* array, int H, int W);
void free_array(array_t *arr);

void overlay_anywhere(array_t *array, int padding, char pchar);
void overlay_unsafe(int y, int x,  array_t *building);
psize_t get_free_area(int h, int w, int p, char pchar);
char check_area_is_free(int y, int x, int h, int w);
void _fill_area_with_char(int y, int x, int h, int w, char pchar);


#endif /* GEN_H */