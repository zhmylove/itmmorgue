#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <regex.h>
#include <math.h>
#include <sys/time.h>

#include "generate.h"

///////////////////////////////////////ARRAY///////////////////////////////////////

void print_array(array_t *arr) {
	
	if (array_is_free(arr)) return;
	
	int i, j;
	int H = arr->size.height, W = arr->size.width;
	char** array = arr->array;
	
	// printf("array_pointer=%lu, H=%d, W=%d\n", (unsigned long)arr, H, W);
	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) 
			printf("%c", array[i][j]);
		printf("\n");
	}
	// printf("\n");
}

void copy_array(array_t* dst, char* array, int H, int W) {
	int i, j;
	init_array(dst, H, W);
	
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++) 
			dst->array[i][j] = *(array + i * W + j);
	
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
	arr->size.height = H;
	arr->size.width = W;
	arr->array = (char**)calloc(H, sizeof(char*));
	if (arr->array == NULL) {
		perror("could not allocate memory for array_t->array\n");
		exit(1);
	}
	for (i = 0; i < H; i++) {
		arr->array[i] = (char*)calloc(W, sizeof(char));
		if (arr->array[i] == NULL) {
			perror("could not allocate memory for array_t->array[%d]\n");
			exit(1);
		}
	}
}

void resize_array(array_t* arr, int H, int W) {
	
	int i, j;
	int min_h = H < arr->size.height ? H : arr->size.height;
	int min_w = W < arr->size.width ? W : arr->size.width;
	
	char **new_array = (char**)malloc(H * sizeof(char*));
	for (i = 0; i < H; i++)
		new_array[i] = (char*)malloc(W * sizeof(char));
	
	for (i = 0; i < min_h; i++) {
		for (j = 0; j < min_w; j++) {
			new_array[i][j] = arr->array[i][j];
		}
	}
	
	free_array(arr);
	
	arr->array = new_array;
	arr->size.height = H;
	arr->size.width = W;
}

void free_array(array_t *arr) {
	
	if (array_is_free(arr)) return;
	
	int i;
	for (i = 0; i < arr->size.height; i++)
		free(arr->array[i]);
	free(arr->array);
	
	arr->size.height = 0;
	arr->size.width = 0;
}

int array_is_free(array_t *arr) {
	if (arr != NULL)
		if (arr->size.height == 0 || arr->size.width == 0) 
			return 1;
	return 0;
}

///////////////////////////////////////GEN///////////////////////////////////////

array_t **WORLD = NULL;
char *qr_free = NULL;
int _SIZE = 0;
int _LEVEL = 0;

// Get or set current level
int level(int lvl) { 
	int curr = (lvl >= 0) ? lvl : _LEVEL;
	if (curr > _SIZE - 1 || _SIZE == 0) {
		WORLD = (array_t**)realloc(WORLD, (curr + 1) * sizeof(array_t*));
		int i;
		for (i = _SIZE; i <= curr; i++)
			WORLD[i] = (array_t*)calloc(1, sizeof(array_t));
		_SIZE = curr + 1;
	}
	_LEVEL = curr;
	return _LEVEL;
}

// Get or set current free regex
void free_regex(char* regex) {
	if (regex == NULL) regex = ".\"^";
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
	
	int curr = (lvl >= 0) ? lvl : _LEVEL;
	if (curr > _SIZE - 1) 
		return NULL;
	return WORLD[curr];
}

// Print the level to stdout
// arg: level number (current level if < 0)
void print_level(int lvl) {
	int curr = (lvl >= 0) ? lvl : _LEVEL;
	if (WORLD[curr] != NULL)
		print_array(WORLD[curr]);
}

// Slurp the level from stdin
// arg: level number (current level if < 0)
array_t* read_level(int lvl) {
   
	int old_level = _LEVEL;
	int curr = (lvl >= 0) ? lvl : _LEVEL;
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
	int curr = (lvl >= 0) ? lvl : _LEVEL;

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
	
	psize_t size = get_size(DEFAULT);
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
	
	do {
		ry = p + (int)rand_double(H - 1 - h - 2 * p);
		rx = p + (int)rand_double(W - 1 - w - 2 * p);
		//printf("ry=%d, rx=%d not ok\n", ry, rx);
	} while (ttl-- > 0 && check_area_is_free(ry - p, rx - p, h + 2 * p, w + 2 * p) == 0);
	
	if (ttl <= 0) {
		print_level(DEFAULT);
		fprintf(stderr, "Unable to get free area\n");
		printf("H=%d, W=%d, h=%d, w=%d, padding=%d, qr_free=%s\n", H, W, h, w, p, qr_free);
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

	if (level >= _SIZE || array_is_free(WORLD[level])) {
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

///////////////////////////////////////BUILD///////////////////////////////////////

// Places 1 near main door.
array_t* _decorate(int edx, int edy, int w, int h, array_t* bldg) {
	
	int sx_left = (edx == 0), sx_right = (edx == w - 1);
	int sy_up   = (edy == 0),  sy_down = (edy == h - 1);
	int sx = sx_left || sx_right;
	int sy = sy_up   || sy_down;
	
	array_t* nbldg = create_array(h + sy, w + sx);
	
	int i, j;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			nbldg->array[i + sy_up][j + sx_left] = bldg->array[i][j];
		}
	}
	if (sy) {
		if (sy_down) edy = h;
		for (i = 0; i < w; i++) nbldg->array[edy][i] = ' ';
		nbldg->array[edy][edx] = '1';
	}
	if (sx) {
		if (sx_right) edx = w;
		for (i = 0; i < h; i++) nbldg->array[i][edx] = ' ';
		nbldg->array[edy][edx] = '1';
	}
	
	free_array(bldg);
	free(bldg);
	
	return nbldg;
}

// Adds some doors to building
void place_doors(int w, int h, array_t* bldg) {
	
	int i, j, k, m;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			if (bldg->array[i][j] == '#' && rand_double(1) < 0.03)
				bldg->array[i][j] = '+'; 
		}
	}
	int cnt;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			if (bldg->array[i][j] == '+') {
				cnt = 0;
				// Doors count
				for (k = i - (i > 0 ? 1 : 0); k <= i + (i < h - 1 ? 1 : 0); k++)
					for (m = j - (j - 1 < 0 ? 0 : 1); m <= j + (j < w - 1 ? 1 : 0); m++)
						if (bldg->array[k][m] == '+')
							cnt++;

				if (cnt != 1) {
					bldg->array[i][j] = '#';
					continue;
				}
				// Door angleness
				cnt = (i == 0     || bldg->array[i - 1][j] == '_') + 
					  (i == h - 1 || bldg->array[i + 1][j] == '_');
				if (cnt == 2) continue;
				
				if (cnt == 1) {
					bldg->array[i][j] = '#';
					continue;
				}
				
				if (2 == (
					(j == 0     || bldg->array[i][j - 1] == '_') +
					(j == w - 1 || bldg->array[i][j + 1] == '_')
				)) continue;
				
				bldg->array[i][j] = '#';
			}
		}
	}
}

// Checks reachability of each non-wall point
int everything_is_reachable(int edy, int edx, int w, int h, array_t* bldg) {

	if (bldg->array[edy][edx] != '+')
		return 0;
	
	int i, j, x, y, q_first = 0, q_last = 0, q_len = 20;
	psize_t	tmp_point; 
	
	psize_t* q;								// Queue for start points
	array_t* visited = create_array(h, w); 	// Map of visited points
	
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
			visited->array[i][j] = 0;
	
	q = malloc(q_len * sizeof(psize_t));
	
	q[q_last].height = edy;
	q[q_last].width = edx;
	q_last++;
	visited->array[edy][edx] = 1;
		
	while (0 != (q_last - q_first)) {
		tmp_point = q[q_first++];
		y = tmp_point.height;
		x = tmp_point.width;
		if (y < 0 || y > h - 1 || x < 0 || x > w - 1)
			continue;

		for (i = y - 1; i <= y + 1; i++) {
			if (i < 0 || i > h - 1) continue;
			for (j = x - 1; j <= x + 1; j++) {
				if (j < 0 || j > w - 1) continue;
				// We've just checked building borders.
				// Now we have to check that we are looking onto door. It means that
				// should check reachability without diagonal tiles.
				if (
					('+' == bldg->array[i][j] || '+' == bldg->array[y][x]) &&
					(abs(x - j) + abs(y - i) != 1)
					) continue;
				if (visited->array[i][j] == 0 && bldg->array[i][j] != '#') {
					q[q_last].height = i;
					q[q_last].width = j;
					q_last++;
					
					if (q_last == q_len) {
						if (q_first > 0) {
							memmove(q, q + q_first, q_last - q_first);
							q_last = q_last - q_first;
							q_first = 0;
						}
						q = realloc(q, (q_len *= 2) * sizeof(psize_t));	
					}
					
					visited->array[i][j] = 1;
				}
			}
		}
	}
	
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			if (bldg->array[i][j] != '#' && visited->array[i][j] == 0)
				return 0;
		}
    }
	
	free_array(visited);
	free(visited);
	free(q);

	return 1;
}

// Returns new two-dimensional array with building.
//
// arg0 : width  
// arg1 : height
// ret  : building
array_t* get_building(int w, int h) {
	
	if (w <= 0) w = 20;
	if (h <= 0) h = 20;
	if (w < 5 || h < 5) {
		fprintf(stderr, "Building width and height must be at least 5 tails\n");
		exit(1);
	}
	
	array_t* bldg = create_array(h, w); // building

	int i, j, k;
	// Initial floor using S_FLOOR
	for (i = 1; i <= h - 2; i++) {
		for (j = 1; j <= w - 2; j++) {
			bldg->array[i][j] = '_';
		}
	}

	// Main walls using S_WALL
	for (i = 0; i < h; i++) {
		bldg->array[i][0] = '#';
		bldg->array[i][w - 1] = '#';
	}
	for (j = 0; j < w; j++) {
		bldg->array[0][j] = '#'; 
		bldg->array[h - 1][j] = '#';
	}
	
	int x, y;
	// Rooms
	for (i = 0; i <= h * 0.11; i++) {
		y = rand() % h;
		for (j = 0; j < w; j++)
			bldg->array[y][j] = '#';
	}

	for (j = 0; j <= w * 0.11; j++) {
		x = rand() % w;
		for (i = 0; i < h; i++)
			bldg->array[i][x] = '#';
	}

	// Get rid of fat walls
	int tries = w * h, wall_fixed = 1;
	while (tries-- > 0 && wall_fixed != 0) {
		wall_fixed = 0;
		for (i = 1; i <= h - 2; i++) {
			for (j = 1; j <= w - 3; j++) {
				if (bldg->array[i][1] != '#') continue;
				// Next line is wall or previous line is wall. And this line is wall.
				if  ('#' == bldg->array[i][j + 1] && 
					(('#' == bldg->array[i - 1][j] && '#' == bldg->array[i - 1][j + 1]) ||
					('#' == bldg->array[i + 1][j] && '#' == bldg->array[i + 1][j + 1]))) {
						
						for (k = 1; k <= w - 2; k++) bldg->array[i][k] = '_';
						wall_fixed = 1;
				}
			}
		}

		for (i = 1; i <= w - 2; i++) {
			for (j = 1; j <= h - 3; j++) {
				if (bldg->array[1][i] != '#') continue;
				// Next line is wall or previous line is wall. And this line is wall.
				if  ('#' == bldg->array[j + 1][i] &&
					(('#' == bldg->array[j][i - 1] && '#' == bldg->array[j + 1][i - 1]) ||
					('#' == bldg->array[j][i + 1] && '#' == bldg->array[j + 1][i + 1]))) {
						for (k = 1; k <= h - 2; k++) bldg->array[k][i] = '_';
				}
			}
		}

		int wpos = -1;
		// Fix leaky walls
		for (i = 1; i <= h - 2; i++) {
			for (j = 1; j <= w - 2; j++) {
				if ('#' == bldg->array[i][j]) {
					if (1 == ('#' == bldg->array[i - 1][j]) + ('#' == bldg->array[i + 1][j])) {
						// Try to fix broken vertical line
						// Find closest wall on the left
						wpos = -1;
						for (k = j - 1; k >= 1; k--) {
							if ('#' == bldg->array[i][k]) {
								wpos = k;
								break;
							}
						}
						if (wpos != -1) {
							// Conduct to the left
							for (k = wpos; k <= j - 1; k++) bldg->array[i][k] = '#';
							wall_fixed = 1;
							continue;
						}

						// Find closest wall on the right
						for (k = j + 1; k <= w - 2; k++) {
							if ('#' == bldg->array[i][k]) {
								wpos = k;
								break; 
							}
						}
						if (wpos != -1) {
							// Conduct to the right
							for (k = j + 1; k <= wpos; k++) bldg->array[i][k] = '#';
							wall_fixed = 1;
							continue;
						}

						// Haven't found, make full vertical line
						wall_fixed = 1;
						for (k = 0; k < h; k++) bldg->array[k][j] = '#';
						continue;
					}

					if (1 == ('#' == bldg->array[i][j - 1]) + ('#' == bldg->array[i][j + 1])) {
						// Try to fix broken horizontal line
						// Find closest wall on the top
						wpos = -1;
						for (k = i - 1; k >= 1; k--) {
							if ('#' == bldg->array[k][j]) {
								wpos = k;
								break;
							}
						}
						if (wpos != -1) {
							// Conduct to the top
							for (k = wpos; k <= i - 1; k++) bldg->array[k][j] = '#';
							wall_fixed = 1;
							continue;
						}

						// Find closest wall on the bottom
						for (k = i + 1; k <= h - 2; k++) {
							if ('#' == bldg->array[k][j]) {
								wpos = k;
								break;
							}
						}
						if (wpos != -1) {
							// Conduct to the bottom
							for (k = i + 1; k <= wpos; k++) bldg->array[k][j] = '#';
							wall_fixed = 1;
							continue;
						}

						// Haven't found, make full horizontal line
						wall_fixed = 1;
						for (k = 0; k < w; k++) bldg->array[i][k] = '#';
					}
				}
			}
		}
	}
	
	int edy, edx; // Enter Door coordinates
	if (rand_double(1) > 0.5) {
		edy = (int)(rand_double(1) * (h - 3)) + 1;
		edx = rand_double(1) > 0.5 ? 0 : w - 1;
	} else {
		edx = (int)(rand_double(1) * (w - 3)) + 1;
		edy = rand_double(1) > 0.5 ? 0 : h - 1;
	}
	bldg->array[edy][edx] = '+';
	
	// Plant some doors
	place_doors(w, h, bldg);
	if (everything_is_reachable(edy, edx, w, h, bldg) != 0) {
		return _decorate(edx, edy, w, h, bldg);
	}
	// Try one more time
	place_doors(w, h, bldg);
	if (everything_is_reachable(edy, edx, w, h, bldg) != 0) {
		return _decorate(edx, edy, w, h, bldg);
	}
	
	free_array(bldg);
	free(bldg);
	
	// Sometime we'll be lucky
	return get_building(w, h);
}

///////////////////////////////////////HOUSE///////////////////////////////////////

array_t _HOUSE = {0};
int _HOUSE_W = 0, _HOUSE_H = 0; 	// width and Height
int bx = 0, by = 0; 				// Builder bot's coordinates

// Replace all undefs with spaces and fix the whole width
static void _normalize_house() {
	
	int x, y;

	// Lookup for max and min defined Y
	int max_y = 0, min_y = INT_MAX;
	for (x = 0; x < _HOUSE.size.height; x++) {
		for (y = 0; y < _HOUSE.size.width; y++) {
			if (y < min_y && _HOUSE.array[x][y] != '\0') min_y = y;
			if (y > max_y && _HOUSE.array[x][y] != '\0') max_y = y ;
		}
	}
	
	// Move the array to the left
	for (x = 0; x < _HOUSE.size.height; x++) {
		memmove(&_HOUSE.array[x][0], &_HOUSE.array[x][min_y], (_HOUSE.size.width - min_y) * sizeof(char));
		// for (i = 0; i < _HOUSE.size.width - min_y; i++) {
			// _HOUSE.array[x][i] = _HOUSE.array[x][min_y + i];			
		// }
	}
	
	resize_array(&_HOUSE, _HOUSE_H, max_y - min_y + 1);
	
	int i, j;
	// Fill the undefs with S_NONE
	for (i = 0; i < _HOUSE.size.height; i++) {
		for (j = 0; j < _HOUSE.size.width /*j <= max_y - min_y*/; j++) {
			if (_HOUSE.array[i][j] == '\0') 
				_HOUSE.array[i][j] = ' ';
		}
	}
}

// (internal, unsafe) Fill specified area with chamber
static void _chamber(int x, int y, int h, int w) {

	if (_HOUSE_H < x + h) _HOUSE_H = x + h;
	if (_HOUSE_W < y + w) _HOUSE_W = y + w;

	char *line = (char*) malloc(w * sizeof(char));
	memset(line, '#', w);
	int i = 0, j;

	for (j = y; j < y + w; j++) {
		if (_HOUSE.array[x][j] <= '\0') _HOUSE.array[x][j] = line[i];
		i++;
	}
	x++;
	h--;
	
	memset(line + 1, '_', w - 2);
	while (h-- > 1) {
		i = 0;
		for (j = y; j < y + w; j++) {
			if (_HOUSE.array[x][j] <= '\0') _HOUSE.array[x][j] = line[i];
			i++;
		}
		x++;
	}
	
	memset(line, '#', w);
	i = 0;
	for (j = y; j < y + w; j++) {
		if (_HOUSE.array[x][j] <= '\0') _HOUSE.array[x][j] = line[i];
		i++;
	}
	free(line);
}

// Build the building
// arg: hashref
//  - : TYPE => type of the building
//  - : ROOMS => number of rooms (not strict)
//  - : width => width (not strict)
//  - : height => height (not strict)
//  - : xfactor and yfactor => room size multiplier
//  - : xmin and ymin => min size of rooms (strict)
array_t* build(builder_t builder) {
	
	char 	TYPE 	= builder.TYPE;
	int 	ROOMS 	= builder.ROOMS 	> 0 ? builder.ROOMS 	: 2;
	int 	width 	= builder.width 	> 0 ? builder.width 	: 10;
	int 	height 	= builder.height 	> 0 ? builder.height 	: 9;
	double 	xfactor = builder.xfactor 	> 0 ? builder.xfactor 	: 2.0;
	double 	yfactor = builder.yfactor	> 0 ? builder.yfactor 	: 1.5;
	int 	xmin 	= builder.xmin 		> 0 ? builder.xmin 		: 4;
	int 	ymin 	= builder.ymin		> 0 ? builder.ymin 		: 4;				
								
	// Initialize
	if (!array_is_free(&_HOUSE)) free_array(&_HOUSE);
	init_array(&_HOUSE, 
				(ROOMS + 2) * (xmin + (height + 1) / xfactor),
					(ROOMS + 2) * (ymin + (width + 1) / yfactor));
	
	_HOUSE_W = _HOUSE_H = 0;
	bx = by = 0;

	// Select entrance position and put the entrance
	bx = 0;
	by = 2 + (int)rand_double(width - 1);
	_HOUSE.array[bx++][by] = '1';
	if (TYPE > 0) _HOUSE.array[bx + 1][by] = TYPE;
	_HOUSE_H++;

	// grow directions: 0 -- left, 1 -- down, 2 -- right
	int direction = 1;

	int rooms = 0;
	int rwidth, rheight, top_y, top_x;
	do {
		// room size
		rwidth = ymin + (int)(rand_double(width + 1) / yfactor);
		rheight = xmin + (int)(rand_double(height + 1) / xfactor);

		// Place the door on the previous coordinates
		_HOUSE.array[bx][by] = '+';

		// Attach new room to the building
		if (direction == 1) {
			top_x = bx;
			do {
				top_y = by - 1 - (int)rand_double(rwidth / 1.5);
				if (by <= 2) top_y = 1;
			} while (top_y < 1);

			if (abs(top_y) - by >= rwidth - 1) rwidth += 2;

			_chamber(top_x, top_y, rheight, rwidth);
		} 
		else if (direction == 2) {
			top_y = by;
			do {
				top_x = bx - 1 - (int)rand_double(rheight / 2.5);
				if (bx <= 2) top_x = 1;
			} while (top_x < 1);
			if (abs(top_x) - bx >= rheight - 1) rheight += 2;

			_chamber(top_x, top_y, rheight, rwidth);
		}

		// Choose a direction
		if (direction == 1) {
			direction = rand_double(2) >= 0.9 ? 2 : 1; // TODO s/1/0/ (really ???)
		} else if (direction == 2) {
			direction = rand_double(2) >= 0.9 ? 1 : 2;
		} else {
			direction = rand_double(2) >= 0.9 ? 1 : 2;
		}

		// Move the builder to new location
		if (direction == 1) {
			bx = top_x + rheight - 1;
			by = top_y + (int)rand_double(rwidth) - 2;
			if (by <= top_y) by = top_y + 1;
		} else if (direction == 2) {
			by = top_y + rwidth - 1;
			bx = top_x + (int)rand_double(rheight) - 2;
			if (bx <= top_x) bx = top_x + 1;
		}
	} while (rand_double(ROOMS + 1) > rooms++);

	// Fix all necessary things
	_normalize_house();

	// Return the house
	return &_HOUSE;
}


///////////////////////////////////////GEN_SURFACE///////////////////////////////////////

//use GD::Simple;

/***
*
* Generate the level.
*
* USAGE:
* ./generate [-h<Height>] [-w<Width>] | ...
*
* ENVIRONMENT:
* Basically, it should be enough to leave the ENVIRONMENT as is.
* Here are available variables:
*
* - TILES=1  (default) stdout <<< tiles, as is
* - RTILES=1           stdout <<< tiles, numbers
* - PNG=1              stdout <<< binary(PNG image)
* - TXT=1              stdout <<< human readable density map
*
***/

// range -- more mountains, less plains (also vertical contraction)
// Height and Width
int range = 40;
int H = 24;
int W = 80;

// levels   -- something for normalization (21 : see gradiens in GD.pm)
// step_min -- keyline distance stepping (aka horizontal contraction)
// step_max
double levels = 0.9;
int step_min = 15;
int step_max = 55;

int line = 0, steps;

// Generate smoooth random line
// arg: array pointer for generated line
void get_line(double* out) {
	
	//double *out = (double*) malloc(W * sizeof(double));
	//memset(out, 0, W * sizeof(double));
	int ptr = 0;
	
	double first = range * rand_double(1);
	double curr = first;
	int /*sign,*/ count;
	double delta, step;

	do {
		//sign = (rand_double(1) >= 0.5) ? -1 : 1;

		count = step_min + step_max * rand_double(1);

		//TODO lower boundary of delta (to exclude short transformations)
		delta = range * rand_double(1);
		step = (double)(delta - curr) / count;

		do {
			out[ptr++] = curr;
			curr += step;
		} while (count-- > 0 && ptr < W);

	} while (ptr < W);
	
	//return out;
}

// Print out any line in human-readable format
void print_line(double *line, double* dst) {
	int i = 0;
	if (levels == 1) {
		for (i = 0; i < W; i++) {
			if (dst == NULL) printf("%02.02f ", line[i]);
			else dst[i] = line[i];
		}
	} 
	else {
		for (i = 0; i < W; i++) {
			if (dst == NULL) printf("%02d ", (int)line[i]);
			else dst[i] = line[i];
		}
	}
	if (dst == NULL) printf("\n");
}

// This function (for now) divides the whole line into S_GRASS and S_TREE,
// and prints it out in parsable format.
void print_tiles(double *line, char* dst) {
	if (levels > 1) {
		fprintf(stderr, "Levels must be less than 1 for print_tiles()\n");
		exit(EXIT_FAILURE);
	}
	// 4 S_TREE
	// 5 S_GRASS
	// 6 S_GRASS
	char S_TREE = '^', S_GRASS = '.', S_FIELD = '"';
	if (getenv("RTILES") != NULL) {
		S_TREE = 4, S_GRASS = 5, S_FIELD = 6;
	}

	//TODO get 0.3 and 0.3 magic constants from the parameters
	
	int i;
	double random, threshold;
	for (i = 0; i < W; i++) {
		random = rand_double(line[i]);
		threshold = rand_double(0.3);
		if (random > threshold + 0.3) {
			if (dst == NULL) printf("%c", S_TREE);
			else dst[i] = S_TREE;
		}
		else if (random < 0.1) { //TODO estimate this magic value
			if (dst == NULL) printf("%c", S_FIELD);
			else dst[i] = S_FIELD;
		} 
		else {
			if (dst == NULL) printf("%c", S_GRASS);
			else dst[i] = S_GRASS;
		}
	}
	if (dst == NULL) printf("\n");
}

int gen_surface(array_t* arr) {
	
	double abs_max = DBL_MIN, abs_min = DBL_MAX;
	// C -- current
	// N -- next
	// D -- deltas
	// S -- steps
	double *C, *N, *D, *S;
	// T -- result reference
	double **T;
	
	C = (double*) malloc(W * sizeof(double));
	N = (double*) malloc(W * sizeof(double));
	D = (double*) malloc(W * sizeof(double));
	S = (double*) malloc(W * sizeof(double));
	
	T = (double**) malloc(H * sizeof(double*));
	int i;
	for (i = 0; i < H; i++) 
		T[i] = (double*) malloc(W * sizeof(double));

	// Let's generate level 0 -- the surface
	//level(0);
	//T = get_level_ref(0);
	
	// first keyline
	get_line(C);
	for (i = 0; i < W; i++)
		T[line][i] = C[i];
	line++;

	double max;

	do {
		get_line(N);
		
		max = DBL_MIN;

		for (i = 0; i < W; i++) {
			D[i] = N[i] - C[i];
			if (D[i] > max) max = D[i];
		}
		steps = 1 + fabs(max);
		for (i = 0; i < W; i++) S[i] = D[i] / steps;

		while (steps-- > 0 && line < H) {
			for (i = 0; i < W; i++) {
				T[line][i] = (C[i] += S[i]);
				if (abs_max < C[i]) abs_max = C[i];
				if (abs_min > C[i]) abs_min = C[i] ;
			}
			line++;
		}
	} while (line < H);

	int j;
	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) {
			T[i][j] = levels * (T[i][j] - abs_min) / abs_max;
		}
	}
	
	char* TXT;
	if ((TXT = getenv("TXT")) != NULL && atoi(TXT) == 1) {
		for (i = 0; i < H; i++)
			print_line(T[i], NULL);
	}

	/***
	*
	* OK. Here we have @T, which is $Hx$W array of scaled probabilities.
	* Let's convert it to tiles (see stuff.h)
	*
	***/

	char* TILES;
	if (((TILES = getenv("TILES")) != NULL && atoi(TILES) == 1) || 
		(getenv("PNG") == NULL && getenv("TXT") == NULL)) {			
		
		for (i = 0; i < H; i++)
			print_tiles(T[i], arr->array[i]);
	}

	/*
	if (1 == ($ENV{PNG} // 0)) {
	   my $img = GD::Simple->new($W, $H);
	   $img->bgcolor("blue");
	   $img->clear();
	   for (my $i = 0; $i < $H; $i++) {
		  for (my $j = 0; $j < $W; $j++) {
			 $img->line($j, $i, $j, $i,
				$img->fgcolor(sprintf("gradient%d", $T->[$i][$j] + 24)));
		  }
	   }

	   binmode STDOUT;
	   print $img->png;
	}
	*/
		
	free(C); free(N); free(D); free(S);
	for (i = 0; i < H; i++) 
		free(T[i]);
	free(T);
	
	return 0;
}

///////////////////////////////////////GEN_FIELDS///////////////////////////////////////

int gen_fields(array_t* arr, int finalize) {
	
	int H = arr->size.height;
	int W = arr->size.width;
	int i, j, k, m;
	
	char **f = arr->array;
	
	int **nf = (int**) calloc(H, sizeof(int*));
	for (i = 0; i < H; i++) 
		nf[i] = (int*) calloc(W, sizeof(int));
	
	int count = 0;
	
	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) {
			for (k = i - (i == 0 ? 0 : 1); k <= (i + (i < H - 1 ? 1 : 0)); k++) {
				count = 0;
				for (m = j - (j == 0 ? 0 : 1); m <= (j + (j < W - 1 ? 1 : 0)); m++) {
					if (f[k][m] == '"')
						count++;
				}
				nf[i][j] += count;
			}
			
			if (f[i][j] == '"')
				nf[i][j]--;
		}	
	}
	
	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) {
			if (finalize) {
				if ((f[i][j] == '"' || f[i][j] == '.') && nf[i][j] <= 2)
					f[i][j] = '.';
			}
			else {
				if (f[i][j] == '"' || f[i][j] == '.')
					f[i][j] = (nf[i][j] >= 4) ? '"' : '.';
			}
		}	
	}
	
	for (i = 0; i < H; i++)
		free(nf[i]);
	free(nf);
	
	return 0;
}

///////////////////////////////////////GEN_CASTLE///////////////////////////////////////

int gen_castle() {
	
	char CASTLE_TOP[14][15] = {
		"###############" ,
		"#_____________#" ,
		"#____#___#____#" ,
		"#____#___#____#" ,
		"#____#___#____#" ,
		"#____#_F_#____#" ,
		"#____#___#____#" ,
		"#____#####____#" ,
		"#_____________#" ,
		"######___######" ,
		"     #___#     " ,
		"^ ^ ^#___#^ ^ ^" ,
		" ^ ^  #_#  ^ ^ " ,
		"^ ^ ^ 111 ^ ^ ^" 
	};

	char CASTLE_FRONT[23][74] = {
		"...........................                    ...........................",
		".#######..#######..#######.                    .#######..#######..#######.",
		".#_____#..#_____#..#_____#.                    .#_____#..#_____#..#_____#.",
		".##___##..##___##..##___##.                    .##___##..##___##..##___##.",
		"..#___#....#___#....#___#........................#___#....#___#....#___#..",
		" .#___######___######___###..####.4####4.####..###___######___######___#. ",
		" .#_______________________#..#__#.4+__+4.#__#..#_______________________#. ",
		" ..#______________________####__####__####__####______________________#.. ",
		"  ..#________________________________________________________________#..  ",
		"   ..#___#####____#####____________________________#####____#####___#..   ",
		"    .#___#___#____#___#____________________________#___#____#___#___#.    ",
		"    .#___#___#____#___#____________________________#___#____#___#___#.    ",
		"    .#___#___#____#___#____________####____________#___#____#___#___#.    ",
		"    .#___#_l_#____#_l_#_________###_##_###_________#_l_#____#_l_#___#.    ",
		"    .#___#___#____#___#_______###___##___###_______#___#____#___#___#.    ",
		"    .#___#####____#####______##_____##_____##______#####____#####___#.    ",
		"    .#_______________________+______##______+_______________________#.    ",
		"    .#_______________________#______##______#_______________________#.    ",
		"    .#_______________________#____#_##_#____#_______________________#.    ",
		"    .#_______________________+______##______+_____________________C_#.    ",
		"    .#_______________________#______##______#_______________________#.    ",
		"    .################################################################.    ",
		"    ..................................................................    "
	};

	char CASTLE_WTF[37][65] = {
		"                             .........                           ",
		"                          ....#######....                        ",
		"                        ...###___l___###...                      ",
		"                       ..##_____________##..                     ",
		"                       .##_______________##.                     ",
		"                      ..#_________________#..                    ",
		"                      .##_________________##.                    ",
		"              .........#___________________#.........            ",
		"             ..###########+#########################..           ",
		"             .#________#____________________________#......      ",
		"             .#_______#_____________________________#......      ",
		"             .#_______+_____________________________#......      ",
		"             .#_______#_______###############_______#......      ",
		"       .......#______#_____###_______###_____####___#.4.....     ",
		"   .....#######______#__###_____________###______#+###+####....  ",
		"  ..#####_____#______###___________________##_______#____#####.. ",
		"...##_____l___#______#_______________________#______+________##..",
		".##___________#______#_______________________#______#__________##",
		".#____________#______#___________#___________#______#___________#",
		".##___________#______#_________##____________#______#__________##",
		"...##_________+______#________##_____________#______#________##..",
		"  ..#####_____#_______##_C__##_____________###______#____#####.. ",
		"   .....########+#______####____________###__#______#######....  ",
		"       .......#___####_____###.......###_____#______#.......     ",
		"             .#_______######________________#_______#.           ",
		"             .#_____________________________#_______#.           ",
		"             .#_____________________________#_______#.           ",
		"             .#____________________________#___l____#.           ",
		"             ..#########################+###########..           ",
		"              .........#___________________#.........            ",
		"                      .##__________l______##.                    ",
		"                      ..#_________________#..                    ",
		"                       .##_______________##.                     ",
		"                       ..##_____________##..                     ",
		"                        ...###_______###...                      ",
		"                          ....#######....                        ",
		"                             .........                           ",
	};
	
	array_t castle_top, castle_front, castle_wtf;
	
	copy_array(&castle_top,(char*)CASTLE_TOP, 14, 15);
	copy_array(&castle_front, (char*)CASTLE_FRONT, 23, 74);
	copy_array(&castle_wtf, (char*)CASTLE_WTF, 37, 65);
	
	array_rotate(&castle_top, DEFAULT);
	overlay_anywhere(&castle_top, 2, ',');
	
	overlay_anywhere(&castle_front, 0, 0);
	
	array_rotate(&castle_wtf, rand_double(1) >= 0.5 ? 0 : 2);
	overlay_anywhere(&castle_wtf, 0, 0);
	
	free_array(&castle_top);
	free_array(&castle_front);
	free_array(&castle_wtf);
	return 0;
}

///////////////////////////////////////GEN_BUILDINGS///////////////////////////////////////

int gen_buildings() {
	
	array_t* building;
	int i;
	for (i = 0; i < 5; i++) {
		building = get_building(rand() % 13 + 6, rand() % 13 + 6);
		overlay_anywhere(building, 2, ',');
		free_array(building);
		free(building);
	}
	return 0;
}

///////////////////////////////////////GEN_CITIES///////////////////////////////////////

int gen_cities() {
	
	int i, j;
	
	// Switch to the first level
	level(1);

	// Get random angle for the city being built
	int city_angle = rand() % 4;

	// Create a new level for city
	int city_factor = 6;
	int size = 24 * city_factor + rand() % city_factor;
	double city_h = 0.4 * size, city_w = 0.6 * size;
	if (city_angle % 2)
		recreate_level_unsafe(city_w, city_h, DEFAULT);
	else
		recreate_level_unsafe(city_h, city_w, DEFAULT);

	// Generate city ground
	generate_blurred_area(1, ',', 0.45);

	// Use ',' as free space too
	free_regex(".,\"^");

	char *TYPES = "STEHG           ";
	int types_p = 0;
	int bh, bw, tmp;
	array_t* building = NULL;
	builder_t builder = {0};
	int rotate;
	do {
		if (rand_double(2) >= 1) {
			building = get_building(rand() % 8 + 5, rand() % 8 + 5);
		}
		else {
			builder.TYPE = TYPES[types_p++];
			building = build(builder);
		}

		// Deny rotation of wide buildings
		bh = building->size.height;
		bw = building->size.width;
		if (bw > bh) {
			tmp = bw;
			bw = bh;
			bh = tmp;
		}
		if ((double)bh / (double)bw >= 1.5) 
			rotate = rand_double(2) >= 1 ? 0 : 2;

		// Overlay the building with rotation
		overlay_somehow(building, rotate, DEFAULT);
	} while (city_factor-- > 0);

	// Place some grass/trees inside the city
	array_t* CITY = get_level_ref(1);
	for (i = 0; i < CITY->size.height; i++) {
		for (j = 0; j < CITY->size.width; j++) {
			if (CITY->array[i][j] == ',' && rand_double(1) >= 0.92) {
				CITY->array[i][j] = rand_double(2) >= 1.5 ? ' ' : '^';
			}
		}
	}

	// Switch back to the main level
	level(0);

	// Reset free space regex
	free_regex(NULL);

	// Overlay generated city over it
	overlay_somehow(CITY, city_angle, DEFAULT);
	
	free_array(CITY);
	free_array(building);
	
	return 0;
}

///////////////////////////////////////GEN_PLACEHOLDERS///////////////////////////////////////

int gen_placeholders(array_t* arr) {
	
	FILE *fp;
	regex_t regex;
	int reti;
	char from[20] = {0};
	char to[20] = {0};
	
	/* Compile regular expression */
	reti = regcomp(&regex, "^(\\w+)\\s+[[](.)[]]\\s+[[](.)[]]\\s+([[](.)[]])?", REG_EXTENDED);
	if (reti) {
		fprintf(stderr, "Could not compile regex\n");
		exit(1);
	}
	
	size_t nmatch = 6;
	regmatch_t pmatch[6];
	
	char *f;
	size_t len = 0;
	ssize_t nread;
	
	if ((fp = fopen("./stuff", "r")) == NULL) {
		fprintf(stderr, "Could not open ./stuff\n");
		exit(1);
	}
	
	int cnt = 0;
	while ((nread = getline(&f, &len, fp)) != -1) {
	
		/* Execute regular expression */
		reti = regexec(&regex, f, nmatch, pmatch, 0);
		if (!reti) {
			if (f[pmatch[5].rm_so]) {
				from[cnt] = f[pmatch[5].rm_so];
				to[cnt] = f[pmatch[2].rm_so];
				cnt++;
			}
		}
	
	}
	
	// char ch; 
	// char* ptr;
	// while ((ch = getc(stdin)) != EOF) {
		// if ((ptr = strchr(from, ch)) != NULL) 
			// putc(to[ptr - from], stdout);
		// else 
			// putc(ch, stdout);
	// }
	
	int i, j;
	char* ptr;
	for (i = 0; i < arr->size.height; i++)
		for (j = 0; j < arr->size.width; j++)
			if ((ptr = strchr(from, arr->array[i][j])) != NULL) 
				arr->array[i][j] = to[ptr - from];
	
	regfree(&regex);
	fclose(fp);
	
	return 0;
}

int generate(char** LEVEL, int H, int W) {
	
	//srand(sysutime());
	
	level(0);
	array_t* T = get_level_ref(0);
	init_array(T, H, W);
	
	gen_surface(T);			// surface creation
	gen_fields(T, 0);		// fields normalization
	gen_fields(T, 1); 		// fields correction
	gen_castle();			// castles creation
	gen_buildings();		// building buildings
	gen_cities();			// building city
	gen_placeholders(T);	// final preparations
	
	int i, j;
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++)
			LEVEL[i][j] = T->array[i][j];
	
	free_array(T);
	free(_get_world_ref());
	
	return 0;
}

int main(int argc, char *argv[]) {

	int i = 0, tmp = 0;
	
	// Parse options
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == 'h') {
			if ((tmp = atoi(&argv[i][2])) > 0)
				H = tmp;
		} 
		else if (argv[i][0] == '-' && argv[i][1] == 'w') {
			if ((tmp = atoi(&argv[i][2])) > 0)
				W = tmp;
		}
		else {
			fprintf(stderr, "Usage: %s -h<Y> -w<X>\n", argv[0]);
			return 1;
		}
	}
	
	struct timeval tv;
	if (gettimeofday(&tv, NULL) < 0) {
        return 1;
    }
    unsigned int seed = tv.tv_sec * 1000000 + tv.tv_usec;
	srand(seed);
	
	
	////array generated in levels.c 
	char** LEVEL;
	LEVEL = (char**)calloc(H, sizeof(char*));
	for (i = 0; i < H; i++) {
		LEVEL[i] = (char*)calloc(W, sizeof(char));
	}
	
	//------->INTEGRATION POINT<-------------
	generate(LEVEL, H, W);
	//------->INTEGRATION POINT<-------------
	
	int j;
	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) 
			printf("%c", LEVEL[i][j]);
		printf("\n");
	}
	
	
	
	return 0;
}
