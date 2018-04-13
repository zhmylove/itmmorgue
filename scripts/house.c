#include <malloc.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "gen.h"
#include "house.h"

array_t HOUSE = {0};
int W = 0, H = 0; 		// Width and Height
int bx = 0, by = 0; 	// Builder bot's coordinates
char TYPE = 0;

// Replace all undefs with spaces and fix the whole width
static void _normalize_house() {
	
	int x, y;

	// Lookup for max and min defined Y
	int max_y = 0, min_y = INT_MAX;
	for (x = 0; x < HOUSE.size.height; x++) {
		for (y = 0; y < HOUSE.size.width; y++) {
			if (y < min_y && HOUSE.array[x][y] != '\0') min_y = y;
			if (y > max_y && HOUSE.array[x][y] != '\0') max_y = y ;
		}
	}
	
	// Move the array to the left
	for (x = 0; x < HOUSE.size.height; x++) {
		memmove(&HOUSE.array[x][0], &HOUSE.array[x][min_y], (HOUSE.size.width - min_y) * sizeof(char));
		// for (i = 0; i < HOUSE.size.width - min_y; i++) {
			// HOUSE.array[x][i] = HOUSE.array[x][min_y + i];			
		// }
	}
	
	resize_array(&HOUSE, H, max_y - min_y + 1);
	
	int i, j;
	// Fill the undefs with S_NONE
	for (i = 0; i < HOUSE.size.height; i++) {
		for (j = 0; j < HOUSE.size.width /*j <= max_y - min_y*/; j++) {
			if (HOUSE.array[i][j] == '\0') 
				HOUSE.array[i][j] = ' ';
		}
	}
}

// Print the hosue to stdout
// void stdout() {
   // print_array(&HOUSE);
// }

// (internal, unsafe) Fill specified area with chamber
static void _chamber(int x, int y, int h, int w) {

	if (H < x + h) H = x + h;
	if (W < y + w) W = y + w;

	char *line = (char*) malloc(w * sizeof(char));
	memset(line, '#', w);
	int i = 0, j;

	for (j = y; j < y + w; j++) {
		if (HOUSE.array[x][j] <= '\0') HOUSE.array[x][j] = line[i];
		i++;
	}
	x++;
	h--;
	
	memset(line + 1, '_', w - 2);
	while (h-- > 1) {
		i = 0;
		for (j = y; j < y + w; j++) {
			if (HOUSE.array[x][j] <= '\0') HOUSE.array[x][j] = line[i];
			i++;
		}
		x++;
	}
	
	memset(line, '#', w);
	i = 0;
	for (j = y; j < y + w; j++) {
		if (HOUSE.array[x][j] <= '\0') HOUSE.array[x][j] = line[i];
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
	
			TYPE 	= builder.TYPE;
	int 	ROOMS 	= builder.ROOMS 	> 0 ? builder.ROOMS 	: 2;
	int 	width 	= builder.width 	> 0 ? builder.width 	: 10;
	int 	height 	= builder.height 	> 0 ? builder.height 	: 9;
	double 	xfactor = builder.xfactor 	> 0 ? builder.xfactor 	: 2.0;
	double 	yfactor = builder.yfactor	> 0 ? builder.yfactor 	: 1.5;
	int 	xmin 	= builder.xmin 		> 0 ? builder.xmin 		: 4;
	int 	ymin 	= builder.ymin		> 0 ? builder.ymin 		: 4;				
								
	// Initialize
	if (!array_is_free(&HOUSE)) free_array(&HOUSE);
	init_array(&HOUSE, 
				(ROOMS + 2) * (xmin + (height + 1) / xfactor),
					(ROOMS + 2) * (ymin + (width + 1) / yfactor));
	
	W = H = 0;
	bx = by = 0;

	// Select entrance position and put the entrance
	bx = 0;
	by = 2 + (int)rand_double(width - 1);
	HOUSE.array[bx++][by] = '1';
	if (TYPE > 0) HOUSE.array[bx + 1][by] = TYPE;
	H++;

	// grow directions: 0 -- left, 1 -- down, 2 -- right
	int direction = 1;

	int rooms = 0;
	int rwidth, rheight, top_y, top_x;
	do {
		// room size
		rwidth = ymin + (int)(rand_double(width + 1) / yfactor);
		rheight = xmin + (int)(rand_double(height + 1) / xfactor);

		// Place the door on the previous coordinates
		HOUSE.array[bx][by] = '+';

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
	return &HOUSE;
}
