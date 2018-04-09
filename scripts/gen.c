#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <regex.h>
#include "gen.h"

#define qr_free(x) (x == '.' || x == '"' || x == '^')

array_t *WORLD = NULL;
int SIZE = 0;
int level = 0;

// (internal) Return a reference to the whole world
array_t* _get_world_ref() {
	return WORLD; 
}

// Return a reference to the level
// arg: level number (current level if < 0)
array_t* get_level_ref(int lvl) {
	
	int curr = (lvl >= 0) ? lvl : level;
	if (curr > SIZE - 1 || SIZE == 0) {
		SIZE = curr + 1;
		WORLD = (array_t*)realloc(WORLD, SIZE * sizeof(array_t));
		//for (int i = SIZE; i < curr + 1; i++)
	}
	return &WORLD[curr];
}

// Print the level to stdout
// arg: level number (current level if < 0)
void print_level(int lvl) {
	int curr = (lvl >= 0) ? lvl : level;
	if (WORLD != NULL)
		print_array(&WORLD[curr]);
}

// Slurp the level from stdin
// arg: level number (current level if < 0)
array_t* read_level(int lvl) {
   
	int curr = (lvl >= 0) ? lvl : level;
	int H = 24;
	int W = 0;
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

   return T;
}

// Just return size[2] of the level
// arg: level number (current level if < 0)
psize_t get_size(int lvl) {
	int curr = (lvl >= 0) ? lvl : level;

	return WORLD[curr].size;
}

void _reverse(array_t *arr) {
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

void _flip(array_t *arr) {
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

void _cw(array_t *arr) {
	int i, j;
	int H = arr->size.height, W = arr->size.width;
	char** array = arr->array;
	
	char **tmp = (char**)malloc(W * sizeof(char*));
	for (i = 0; i < W; i++)
		tmp[i] = (char*)malloc(H * sizeof(char));
	
	for (i = W - 1; i >= 0; i--) {
		for (j = 0; j < H; j++) {
			tmp[i][j] = array[j][i];
		}
	}
	
	// if (W < H) {
		// for (i = W; i < H; i++)
			// free(array[i]);
	// }
	// array = (char**)realloc(array, W * sizeof(char*));
	// for (i = 0; i < W; i++)
		// array[i] = (char*)realloc(array[i], H * sizeof(char));
	
	// for (i = 0; i < W; i++) 
		// for (j = 0; j < H; j++) 
			// array[i][j] = tmp[i][j];
	
	
	for (i = 0; i < H; i++)
		free(array[i]); 
	free(array);
	
	arr->array = tmp;
	
	arr->size.width = H;
	arr->size.height = W;
}

void array_rotate(array_t *array, int direction) {
	
	if (direction < 0 || direction > 3) {
		srand(time(NULL));
		direction = rand() % 4;
	}
	if (!direction)
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

void print_array(array_t *arr) {
	int i, j;
	int H = arr->size.height, W = arr->size.width;
	char** array = arr->array;
	
	//printf("array_pointer=%lu, H=%d, W=%d\n", (unsigned long)arr, H, W);
	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) 
			printf("%c", array[i][j]);
		printf("\n");
	}
	//printf("\n");
}

array_t* copy_array(char* array, int H, int W) {
	int i, j;
	array_t* copied = (array_t*) malloc(sizeof(array_t));
	copied->size.height = H;
	copied->size.width = W;
	copied->array = (char**)malloc(H * sizeof(char*));
	for (i = 0; i < H; i++)
		copied->array[i] = (char*)malloc(W * sizeof(char));
	
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++) 
			copied->array[i][j] = *(array + i * W + j);
	
	return copied;
}

array_t* create_array(int H, int W) {
	
	array_t* arr = (array_t*)malloc(sizeof(array_t));
	if (arr == NULL) {
		perror("could not allocate memory for array_t\n");
		exit(1);
	}
	init_array(arr, H, W);
	
	return arr;
}

void init_array(array_t* arr, int H, int W) {
	int i;
	// arr = (array_t*)malloc(sizeof(array_t));
	// if (arr == NULL) {
		// perror("could not allocate memory for array_t\n");
		// exit(1);
	// }
	arr->size.height = H;
	arr->size.width = W;
	arr->array = (char**)malloc(H * sizeof(char*));
	if (arr->array == NULL) {
		perror("could not allocate memory for array_t->array\n");
		exit(1);
	}
	for (i = 0; i < H; i++) {
		arr->array[i] = (char*)malloc(W * sizeof(char));
		if (arr->array[i] == NULL) {
			perror("could not allocate memory for array_t->array[%d]\n");
			exit(1);
		}
	}
	
	// return arr;
}

void free_array(array_t *arr) {
	int i;
	for (i = 0; i < arr->size.height; i++)
		free(arr->array[i]);
	free(arr->array);
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
	//int h = building->size.height - 1;
	
	array_t *T = get_level_ref(-1);
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
	if (pchar != '\0')
		_fill_area_with_char(ry - p, rx - p, h + 2 * p, w + 2 * p, pchar);

	size.height = ry;
	size.width = rx;
	return size;
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
	
	array_t *T = get_level_ref(-1);

	int i, j;
	
	for (i = y; i < y + h; i++) {
		for (j = x; j < x + w; j++) {
			if (qr_free(T->array[i][j]))
				continue;
			else
				return 0;
		}
	}

	return 1;
}

// (internal, unsafe) Fill specified area with char
void _fill_area_with_char(int y, int x, int h, int w, char pchar) {
	if (x < 0 || y < 0 || h < 0 || w < 0 || pchar == '\0') {
		fprintf(stderr, 
			"Wrong arguments in _fill_area_with_char(int y = %d, int x = %d, int h = %d, int w = %d, char pchar = %d)\n",
				y, x, h, w, (int)pchar);
		exit(1);
	}
	
	int i, j;
	array_t *T = get_level_ref(-1);
	
	for (i = y; i < y + h; i++) {
		for (j = x; j < x + w; j++) {
			T->array[i][j] = pchar;
		}
	}
}