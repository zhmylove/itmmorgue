#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "array.h"
#include "gen.h"
#include "build.h"


int main(int argc, char *argv[]) {
	
	array_t* T = read_level(-1);
	srand(time(NULL));
	
	overlay_anywhere(get_building(rand() % 13 + 6, rand() % 13 + 6), 2, ',');
	overlay_anywhere(get_building(rand() % 13 + 6, rand() % 13 + 6), 2, ',');
	overlay_anywhere(get_building(rand() % 13 + 6, rand() % 13 + 6), 2, ',');
	overlay_anywhere(get_building(rand() % 13 + 6, rand() % 13 + 6), 2, ',');
	overlay_anywhere(get_building(rand() % 13 + 6, rand() % 13 + 6), 2, ',');
	
	print_level(-1);
	
	free_array(T);
	free(_get_world_ref());
	
	return 0;
}
