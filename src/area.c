// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "windows.h"
#include "stuff.h"

chtype *area;

// TODO need to obtain **area from server
void area_init() {
    if ((area = malloc(sizeof(chtype) * 80 * 24)) == NULL) {
        panic("Unable to allocate area memory!");
    }

    int i = 0;
    area[i++] = S[S_NONE];
    area[i++] = S[S_PLAYER];
    area[i++] = S[S_WALL];
    area[i++] = S[S_WALL];
    area[i++] = S[S_DTREE];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_LTREE];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_DGRASS];
    area[i++] = S[S_LGRASS];
    area[i++] = S[S_LGRASS];
    area[i++] = S[S_LGRASS];
    area[i++] = S[S_LGRASS];
    area[i++] = '\0';
}

void draw_area() {
    if (area == NULL) {
        return;
    }

    int ptr = 0;
    while (area[ptr] != '\0') {
        mvwaddch(W(W_AREA), 2, ptr + 1, area[ptr++]);
    }
    return;
}

