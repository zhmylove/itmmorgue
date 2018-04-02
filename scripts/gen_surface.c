#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <malloc.h>
//#include <string.h>
#include <math.h>

#define rand_double(x) ((double)rand() / RAND_MAX * x)

//#include "gen.h"

//use GD::Simple;

/***
*
* Generate the surface. It's the initial script for terra generation process.
*
* USAGE:
* ./gen_surface [-h<Height>] [-w<Width>] | ...
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

// C -- current
// N -- next
// D -- deltas
// S -- steps
double *C, *N, *D, *S;
// T -- result reference
double **T;
int line = 0, steps;
double abs_max = DBL_MIN, abs_min = DBL_MAX;

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
void print_line(double *line) {
	int i = 0;
	if (levels == 1) {
		for (i = 0; i < W; i++)
			printf("%02.02f ", line[i]);
	} 
	else {
		for (i = 0; i < W; i++)
			printf("%02d ", (int)line[i]);
	}
	printf("\n");
}

// This function (for now) divides the whole line into S_GRASS and S_TREE,
// and prints it out in parsable format.
void print_tiles(double *line) {
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
			printf("%c", S_TREE);
		} 
		else if (random < 0.1) { //TODO estimate this magic value
			printf("%c", S_FIELD);
		} 
		else {
			printf("%c", S_GRASS);
		}
	}
	printf("\n");
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
	
	C = (double*) malloc(W * sizeof(double));
	N = (double*) malloc(W * sizeof(double));
	D = (double*) malloc(W * sizeof(double));
	S = (double*) malloc(W * sizeof(double));
	
	T = (double**) malloc(H * sizeof(double*));
	for (i = 0; i < H; i++) 
		T[i] = (double*) malloc(W * sizeof(double));

	// Let's generate level 0 -- the surface
	//level(0);
	//T = get_level_ref(0);

	srand(time(NULL));
	
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
			print_line(T[i]);
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
			print_tiles(T[i]);
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
