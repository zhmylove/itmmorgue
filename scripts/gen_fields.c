#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int main(int argc, char *argv[]) {
	
	int H = 24;
	int W = 0;
	
	int i, j, k, m, finalize = 0;
	
	char **f = (char**) malloc((H = H + 1) * sizeof(char*));
	
	size_t len = 0;
	ssize_t nread;
	
	i = 0;
	while ((nread = getline(&f[i], &len, stdin)) != -1) {
		if (nread > W) W = nread;
		i++;
		if (i == H) {
			f = (char**) realloc(f, (H = 2 * H - 1) * sizeof(char*));
		}
	}
	
	if (H + 1 > i) {
		f = (char**) realloc(f, i * sizeof(char*));
		H = i;
	}
	
	int **nf = (int**) malloc(H * sizeof(int*));
	for (i = 0; i < H; i++) 
		nf[i] = (int*) malloc((W - 1) * sizeof(int));
	
	int count = 0;
	
	for (i = 0; i < H; i++) {
		for (j = 0; j < W - 1; j++) {
			for (k = i - (i > 0 ? 1 : 0); k <= (i + (i < H - 1 ? 1 : 0)); k++) {
				count = 0;
				for (m = j - (j - 1 < 0 ? 0 : 1); m <= (j + (j < W - 2 ? 1 : 0)); m++) {
					if (f[k][m] == '"')
						count++;
				}
				nf[i][j] += count;
			}
			
			if (f[i][j] == '"')
				nf[i][j]--;
		}	
	}
	
	if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'f')
		finalize = 1;
	
	for (i = 0; i < H; i++) {
		for (j = 0; j < W - 1; j++) {
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
	
	for (i = 0; i < H; i++) {
		for (j = 0; j < W - 1; j++) {
			printf("%c", f[i][j]);
		}
		printf("\n");
	}
	
	
	for (i = 0; i < H; i++) {
		free(f[i]);
		free(nf[i]);
	}
	free(f);
	free(nf);
	return 0;
}

