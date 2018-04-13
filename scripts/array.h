#ifndef ARRAY_H
#define ARRAY_H

typedef struct psize_t {
	int 	height;
	int 	width;
} psize_t;

typedef struct array_t {
	char** 		array;
	psize_t 	size;
} array_t;

void print_array(array_t *arr);
array_t* copy_array(char* array, int H, int W);
array_t* create_array(int H, int W);
void init_array(array_t* arr, int H, int W);
void resize_array(array_t* arr, int H, int W);
void free_array(array_t *arr);
int array_is_free(array_t *arr);


#endif /* ARRAY_H */