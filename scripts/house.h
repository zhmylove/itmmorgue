#ifndef HOUSE_H
#define HOUSE_H

#include "array.h"

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

//void stdout();
			
#endif /* HOUSE_H */