#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include "array.h"
#include "gen.h"
#include "build.h"

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
	
	// static int recurse = 0;
	// recurse++;
	
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
