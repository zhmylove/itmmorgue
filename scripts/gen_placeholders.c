#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>        

int main(int argc, char *argv[]) {
	
	FILE *fp;
	regex_t regex;
	int reti;
	char from[20] = {0};
	char to[20] = {0};
	
	/* Compile regular expression */
	reti = regcomp(&regex, "^(\\w+)\\s+[[](.)[]]\\s+[[](.)[]]\\s+([[](.)[]])?", REG_EXTENDED);
	if (reti) {
		fprintf(stderr, "Could not compile regex\n");
		exit(1);
	}
	
	size_t nmatch = 6;
	regmatch_t pmatch[6];
	
	char *f;
	size_t len = 0;
	ssize_t nread;
	
	if ((fp = fopen("./stuff", "r")) == NULL) {
		fprintf(stderr, "Could not open ./stuff\n");
		exit(1);
	}
	
	int cnt = 0;
	while ((nread = getline(&f, &len, fp)) != -1) {
	
		/* Execute regular expression */
		reti = regexec(&regex, f, nmatch, pmatch, 0);
		if (!reti) {
			if (f[pmatch[5].rm_so]) {
				from[cnt] = f[pmatch[5].rm_so];
				to[cnt] = f[pmatch[2].rm_so];
				cnt++;
			}
			// printf("Match, \t\t\t%s\n", f);
			// int j;
			// for (j = 0; j < nmatch; j++)
				// printf("pmatch[%d]=%.*s\n", j, pmatch[j].rm_eo - pmatch[j].rm_so, &f[pmatch[j].rm_so]);
		}
	
	}
	
	char ch; 
	char* ptr;
	while ((ch = getc(stdin)) != EOF) {
		if ((ptr = strchr(from, ch)) != NULL) 
			putc(to[ptr - from], stdout);
		else 
			putc(ch, stdout);
	}
	
	regfree(&regex);
	if (fp) fclose(fp);
	return 0;
}