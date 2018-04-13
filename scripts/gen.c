#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>

#include "array.h"
#include "gen.h"

array_t **WORLD = NULL;
char *qr_free = NULL;
int SIZE = 0;
int LEVEL = 0;

// Get or set current level
int level(int lvl) { 
	int curr = (lvl >= 0) ? lvl : LEVEL;
	if (curr > SIZE - 1 || SIZE == 0) {
		WORLD = (array_t**)realloc(WORLD, (curr + 1) * sizeof(array_t*));
		int i;
		for (i = SIZE; i <= curr; i++)
			WORLD[i] = (array_t*)calloc(1, sizeof(array_t));
		SIZE = curr + 1;
	}
	LEVEL = curr;
	return LEVEL;
}

// Get or set current free regex
void free_regex(char* regex) {
	if (regex == NULL) regex = ".\"^";
	//size_t len = strlen(regex);
	qr_free = (char*)realloc(qr_free, strlen(regex) * sizeof(char));
	strcpy(qr_free, regex);
}


// (internal) Return a reference to the whole world
array_t** _get_world_ref() {
	return WORLD; 
}

// Return a reference to the level
// arg: level number (current level if < 0)
array_t* get_level_ref(int lvl) {
	
	int curr = (lvl >= 0) ? lvl : LEVEL;
	if (curr > SIZE - 1) 
		return NULL;
	return WORLD[curr];
}

// Print the level to stdout
// arg: level number (current level if < 0)
void print_level(int lvl) {
	int curr = (lvl >= 0) ? lvl : LEVEL;
	if (WORLD[curr] != NULL)
		print_array(WORLD[curr]);
}

// Slurp the level from stdin
// arg: level number (current level if < 0)
array_t* read_level(int lvl) {
   
	int old_level = LEVEL;
	int curr = (lvl >= 0) ? lvl : LEVEL;
	int H = 24;
	int W = 0;
	
	level(curr);
	array_t* T = get_level_ref(curr);
	
	T->array = (char**)malloc((H = H + 1) * sizeof(char*));
	
	size_t len = 0;
	ssize_t nread;
	
	int i = 0;
	while ((nread = getline(&T->array[i], &len, stdin)) != -1) {
		if (nread > W) W = nread;
		i++;
		if (i == H) {
			T->array = (char**)realloc(T->array, (H = 2 * H - 1) * sizeof(char*));
		}
	}
	
	if (H + 1 > i) {
		T->array = (char**)realloc(T->array, i * sizeof(char*));
		H = i;
	}
	
	T->size.height = H;
	T->size.width  = W - 1;

	level(old_level);
	
	return T;
}

// Just return size[2] of the level
// arg: level number (current level if < 0)
psize_t get_size(int lvl) {
	int curr = (lvl >= 0) ? lvl : LEVEL;

	return WORLD[curr]->size;
}

static void _reverse(array_t *arr) {
	int i, j;
	char tmp;
	int H = arr->size.height, W = arr->size.width;
	char** array = arr->array;
	
	for (i = 0; i < H; i++)
		for (j = 0; j < W / 2; j++) {
			tmp = array[i][j];
			array[i][j] = array[i][W - 1 - j];
			array[i][W - 1 - j] = tmp;
		}
}

static void _flip(array_t *arr) {
	int i;
	char* tmp;
	int H = arr->size.height;
	char** array = arr->array;	
	
	for (i = 0; i < H / 2; i++) {
		tmp = array[i];
		array[i] = array[H - 1 - i];
		array[H - i - 1] = tmp;
	}
}

static void _cw(array_t *arr) {
	int i, j;
	int H = arr->size.height, W = arr->size.width;
	
	char **tmp = (char**)malloc(W * sizeof(char*));
	for (i = 0; i < W; i++)
		tmp[i] = (char*)malloc(H * sizeof(char));
	
	for (i = W - 1; i >= 0; i--) {
		for (j = 0; j < H; j++) {
			tmp[i][j] = arr->array[j][i];
		}
	}
	
	free_array(arr);
	
	arr->array = tmp;
	
	arr->size.width = H;
	arr->size.height = W;
}

void array_rotate(array_t *array, int direction) {
	
	if (direction < 0 || direction > 3) {
		//srand(time(NULL));
		direction = rand() % 4;
	}
	if (direction == 0)
		return;
	
	if (direction == 1) {
		_cw(array);
		_reverse(array);
	}
	else if (direction == 2) {
		_flip(array);
		_reverse(array);
	}
	else {
		_cw(array);
		_flip(array);
	}
}

// Overlay arg1 array over current level on free space with random rotation
// arg1: array reference
// arg2: rotate   => ARRAY (array_rotate)
// arg3: overlay  => ARRAY (overlay_anywhere)
void overlay_somehow(array_t* array, int rotate, int overlay) {

	array_rotate(array, rotate);
	overlay_anywhere(array, overlay, DEFAULT);
}

// Overlay arg1 array over current level on free space
// arg1: 2d array reference
// arg2: padding
// arg3: padding character
void overlay_anywhere(array_t *array, int padding, char pchar) {
	
	psize_t size;
	size = get_free_area(array->size.height, array->size.width, padding, pchar);
	overlay_unsafe(size.height, size.width, array);
}

// Overlay arg3 array over current level at arg1 x arg2 position.
// arg1: Y coordinate of left upper corner
// arg2: X
// arg3: building being built (array ref)
void overlay_unsafe(int y, int x,  array_t* building) {
	
	array_t *T = get_level_ref(DEFAULT);
	int i, j;
	
	for (i = 0; i < building->size.height; i++) {
		for (j = 0; j < building->size.width; j++) {
			if (building->array[i][j] != ' ' && building->array[i][j] != '\0')
				T->array[y + i][x + j] = building->array[i][j];
		}
	}
}

// Get free area on the current level
// arg1: area height
// arg2: area width
// arg3: area padding
// arg4: padding character
psize_t get_free_area(int h, int w, int p, char pchar) {
	
	// default padding is 0
	if (p < 0) p = 0;

	if (h < 0 || w < 0) {
		fprintf(stderr, "Invalid height or width of area\n");
		exit(1);
	}
	
	psize_t size = get_size(-1);
	int H = size.height, W = size.width;

	if ((H < h + 2 * p) || (W < w + 2 * p)) {
		fprintf(stderr, 
			"Level (%d.%d) is too small for area (%d.%d) with padding %d!\n",
				W, H, w, h, p);
		exit(1);
	}

	// random X and Y on the level
	int rx, ry;

	// TODO correct ttl value
	int ttl = 1000;
	
	srand(time(NULL));

	do {
		ry = p + (int)rand_double(H - 1 - h - 2 * p);
		rx = p + (int)rand_double(W - 1 - w - 2 * p);
	} while (ttl-- > 0 && check_area_is_free(ry - p, rx - p, h + 2 * p, w + 2 * p) == 0);
	
	if (ttl <= 0) {
		fprintf(stderr, "Unable to get free area\n");
		exit(1);
	}
		
	// fill padding area if character specified
	// TODO fill only padding (be careful with S_NONE)
	if (pchar > 0)
		_fill_area_with_char(ry - p, rx - p, h + 2 * p, w + 2 * p, pchar);

	size.height = ry;
	size.width = rx;
	return size;
}

// Recreate level and fill it with specified character
// arg1: 	Y size of the level
// arg2: 	X size of the level
// arg3: 	a character to fill the level with
void recreate_level_unsafe(int y, int x, char pchar) {
   
	array_t* T = get_level_ref(DEFAULT);
	if (pchar <= 0) pchar = ' ';

	// Feast for garbage collector
	free_array(T);
	init_array(T, y, x);

	int i, j;
	// Fill entire level
	for (i = 0; i < T->size.height; i++)
		for (j = 0; j < T->size.width; j++)
			T->array[i][j] = pchar;
}

// Check if area is free
// arg1: area Y position
// arg2: area X position
// arg3: area height
// arg4: area width
char check_area_is_free(int y, int x, int h, int w) {
	if (x < 0 || y < 0 || h < 0 || w < 0) {
		fprintf(stderr, 
			"Wrong arguments in check_area_is_free(int y = %d, int x = %d, int h = %d, int w = %d)\n",
				y, x, h, w);
		exit(1);
	}
	
	array_t *T = get_level_ref(DEFAULT);
	
	if (qr_free == NULL) free_regex(NULL);
	
	int i, j;
	
	for (i = y; i < y + h; i++) {
		for (j = x; j < x + w; j++) {
			if (strchr(qr_free, T->array[i][j]) == NULL)
				return 0;
		}
	}

	return 1;
}

// (internal, unsafe) Fill specified area with char
void _fill_area_with_char(int y, int x, int h, int w, char pchar) {
	if (x < 0 || y < 0 || h < 0 || w < 0 || pchar <= 0) {
		fprintf(stderr, 
			"Wrong arguments in _fill_area_with_char(int y = %d, int x = %d, int h = %d, int w = %d, char pchar = %d)\n",
				y, x, h, w, (int)pchar);
		exit(1);
	}
	
	int i, j;
	array_t *T = get_level_ref(DEFAULT);
	
	for (i = y; i < y + h; i++) {
		for (j = x; j < x + w; j++) {
			T->array[i][j] = pchar;
		}
	}
}

// Generate smooth random line y(0) = 0 and y($) = 0
// arg1: length
// arg2: start_solid
// arg3: stop_solid
// arg4: char
static char* _get_line(char* L, int length, int start_solid, int stop_solid, char pchar) {
	
	memset(L, ' ', length);
	int ptr = 0;
	
	double curr = 0;

	int magic_constant = 3;

	// Left side (before solid block)
	while (ptr < start_solid) {
		L[ptr++] = (curr >= magic_constant) ? pchar : ' ';
		curr += rand_double(start_solid) / start_solid;
	}

	// Solid block
	while (ptr < stop_solid) {
		L[ptr++] = pchar; 
	}

	curr = 2 * magic_constant;

	// Right side (after solid block)
	while (ptr < length) {
		L[ptr++] = (curr >= magic_constant) ? pchar : ' ';
		curr -= rand_double(start_solid) / start_solid;
	}

	return L;
}

array_t* generate_blurred_area(int level, char pchar, double factor) {

	if (level >= SIZE || array_is_free(WORLD[level])) {
		fprintf(stderr, "Level %d does not exist\n", level);
		exit(1);
	}
	
	if (factor < 0 || factor > 1) {
		fprintf(stderr, "Invalid factor = %.2f <> (0..1)\n", factor);
		exit(1);
	}
	   
	psize_t size = get_size(level);
	int h = size.height, w = size.width;

	double sy = 0.5 * (1 - factor) * h;
	double sx = 0.5 * (1 - factor) * w;

	array_t* T = get_level_ref(DEFAULT);

	array_rotate(T, 1);
	
	int x, y;
	char *L = (char*)malloc(h * sizeof(char));
	
	for (x = sx / 2; x < w - sx / 2; x++) {
		_get_line(L, h, sy, h - sy, pchar);
		memcpy(T->array[x], L, h);
	}

	array_rotate(T, 3);

	L = (char*)realloc(L, w * sizeof(char));
	
	for (y = sy; y < h - sy; y++) {
		_get_line(L, w, sx, w - sx, pchar);
		memcpy(T->array[y], L, w);
	}
	
	free(L);

	return T;
}