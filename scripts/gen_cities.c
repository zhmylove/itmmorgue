
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

#include "array.h"
#include "gen.h"
#include "build.h"
#include "house.h"

int main(int argc, char *argv[]) {
	
	srand(time(NULL));
	int i, j;
	
	// Read zero (main) level into memory from STDIN
	array_t* T = read_level(0);
	
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

	// Print the level
	print_level(0);
	
	free_array(T);
	free_array(CITY);
	free_array(building);
	free(_get_world_ref());
	
	return 0;
}
