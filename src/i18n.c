// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "client.h"
#include "config.h"

char *strings;
int size = 0;

void i18n_init(char *file) {
    struct stat sb;
    int fd, rc;

    if (file == NULL || ! *file) {
        panic("Invalid file specified for locale!");
    }

    if (stat(file, &sb) < 0) {
        panicf("Unable to get info : %s!", file);
    }

    // TODO fix
#define LOCALE_SIZE_MAX (16 * 1048576)
    if (sb.st_size <= 0 || sb.st_size > LOCALE_SIZE_MAX) {
        panicf("Specified locale file has invalid size: %ld!", sb.st_size);
    }

    if ((strings = malloc(sb.st_size)) == NULL) {
        panic("Unable to allocate memory for l10n file!");
    }

    if ((fd = open(file, O_RDONLY, 0)) < 0) {
        panic("Unable to open l10n file!");
    }

    while (sb.st_size > 0) {
        if ((rc = read(fd, strings + size, sb.st_size)) < 0) {
            panic("Error during reading l10n file!");
        }
        size += rc;
        sb.st_size -= rc;
    }

    warn("Performant index of i18n not implemented yet!");

    // 0 -- parsing of original string
    // 1 -- parsing of localized string
    int state = 0;
    for (int i = 0; i < size; i++) {
        switch (strings[i]) {
            case '\\':
                (void)strings[++i]; //TODO add to index
                continue;
            case '}':
                state ^= 1;
            case '{':
                strings[i] = 0;
        }
    }
}

// TODO total rewrite w/ trie & utf-8
int get_from_index(char *str) {
    int curr = 0;

    while (curr < size) {
        if (strings + curr == strstr(strings + curr, str)) {
            curr += strlen(strings + curr);
            while (curr < size && ! strings[++curr]);

            return curr;
        }

        curr += strlen(strings + curr) + 1;
    }

    return -1;
}

char *_(char *str) {
    // TODO prepare hashmap or index

    if (strings == NULL) 
        return str;

    int position;
    if ((position = get_from_index(str)) >= 0) {
        return strings + position;
    }

    return str;
}
