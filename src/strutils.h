#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// Wrapper for strtol(3) for int numbers
int strtoi(const char *nptr, char **endptr, int base);

// strlen() alternatives for UTF-8 strings
size_t anystrlen(char *str);
size_t anystrnlen(char *str, size_t maxlen);
size_t anystrnplen(char *str, size_t maxlen, char ** endp);
size_t anystrunplen(char *str, size_t maxlen, char ** endp);

#ifdef __sun
size_t strnlen(const char *str, size_t maxlen);
#endif /* __sun */

#endif /* STRING_UTILS_H */
