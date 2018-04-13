#include <stdlib.h>
#include <time.h>

#include "array.h"
#include "build.h"


int main(int argc, char *argv[]) {
	
	srand(time(NULL));
	array_t* bldg = get_building(rand() % 13 + 6, rand() % 13 + 6);	
	print_array(bldg);
	
	free_array(bldg);
	return 0;
}
