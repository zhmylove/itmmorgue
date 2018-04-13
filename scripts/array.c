#include <stdlib.h>
#include <malloc.h>

#include "array.h"


void print_array(array_t *arr) {
	
	if (array_is_free(arr)) return;
	
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
	init_array(copied, H, W);
	
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
	arr->size.height = H;
	arr->size.width = W;
	arr->array = (char**)calloc(H, sizeof(char*));
	if (arr->array == NULL) {
		perror("could not allocate memory for array_t->array\n");
		exit(1);
	}
	for (i = 0; i < H; i++) {
		arr->array[i] = (char*)calloc(W, sizeof(char));
		//memset(arr->array[i], ' ', W);
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