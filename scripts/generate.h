#ifndef GENERATE_H
#define GENERATE_H

////////////////////////////////////////////////ARRAY.H//////////////////////////////////////////////////

typedef struct psize_t {
	int 	height;
	int 	width;
} psize_t;

typedef struct array_t {
	char** 		array;
	psize_t 	size;
} array_t;

void print_array(array_t *arr);
void copy_array(array_t* dst, char* array, int H, int W);
array_t* create_array(int H, int W);
void init_array(array_t* arr, int H, int W);
void resize_array(array_t* arr, int H, int W);
void free_array(array_t *arr);
int array_is_free(array_t *arr);

////////////////////////////////////////////////GEN.H//////////////////////////////////////////////////

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

////////////////////////////////////////////////BUILD.H//////////////////////////////////////////////////

array_t* get_building(int w, int h);
array_t* _decorate(int edx, int edy, int w, int h, array_t* bldg);
void place_doors(int w, int h, array_t* bldg);
int everything_is_reachable(int edy, int edx, int w, int h, array_t* bldg);

////////////////////////////////////////////////HOUSE.H//////////////////////////////////////////////////

typedef struct builder_t {
	char 	TYPE;
	int 	ROOMS;
	int 	width;
	int 	height;
	double 	xfactor;
	double 	yfactor;
	int 	xmin;
	int 	ymin;
	
} builder_t;

array_t* build(builder_t builder);

////////////////////////////////////////////////GENERATE.H//////////////////////////////////////////////////

int gen_surface(array_t* arr);
int gen_fields(array_t* arr, int finalize);
int gen_castle();
int gen_buildings();
int gen_cities();
int gen_placeholders(array_t* arr);
int generate(char** LEVEL, int H, int W);
						
#endif /* GENERATE_H */