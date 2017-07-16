// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "windows.h"
#include "client.h"
#include "sysmsg.h"
#include "keyboard.h"
#include "windows.h"

void c_sysmsg_init() {
    if ((sysmsg = malloc(SYSMSG_MAXLEN + 1)) == NULL) {
        panic("Unable to allocate sysmsg memory!");
    }
    
    sysmsg[0] = '\0';
}

void draw_sysmsg() {
    if (sysmsg == NULL) {
        return;
    }

    int square = WIN(SYSMSG, max_x) * WIN(SYSMSG, max_y);
    size_t len = strlen(sysmsg);
    char *sysmsgptr = sysmsg + len - 2;

    if (len < 2) {
        return;
    }

    int curr = 0;
    while (sysmsgptr > sysmsg && curr < square) {
        switch (*sysmsg) {
            case '\n':
                curr += WIN(SYSMSG, max_x)
                    - curr % WIN(SYSMSG, max_x);
                break;
            default:
                if ((*sysmsgptr & 0xC0) != 0x80) {
                    curr++;
                }
        }
        sysmsgptr--;
    }

    if (sysmsgptr != sysmsg && *(sysmsgptr - 1) != '\n') {
        while (*sysmsgptr++ != '\n');
    }

    MVW(W_SYSMSG, 0, 0, "%s", sysmsgptr);
}

void s_sysmsg_add(char **ssysmsg, char *str) {
    int oldsize = strlen(*ssysmsg) + 1;
    int newsize = oldsize + strlen(str) + 1;

    // TODO: actualize this todos
    // TODO take care of CHAT_MSG_BACKLOG
    // TODO do not forget that this routine is used by both client and server

    if ((*ssysmsg = realloc(*ssysmsg, newsize)) == NULL) {
        panic("Error reallocating server sysmsg buffer!");
    }

    strcat(*ssysmsg, str);
}

void c_sysmsg_add(char *str) {
    int oldsize = strlen(sysmsg) + 1;
    int newsize = oldsize + strlen(str) + 1;

    // TODO: actualize this todos
    // TODO take care of CHAT_MSG_BACKLOG
    // TODO do not forget that this routine is used by both client and server

    if ((sysmsg = realloc(sysmsg, newsize)) == NULL) {
        panic("Error reallocating sysmsg buffer!");
    }

    strcat(sysmsg, str);
}

void c_sysmsg_open() {
    int state_old = WIN(SYSMSG, state);
    int focus_old = focus;

    WIN(SYSMSG, state) = LARGE;
    focus = W_SYSMSG;
    windows_fill(W_SYSMSG, 1);

    do {
        CHECK_CONNECTION();

        windows_redraw();

        wtimeout(W(W_SYSMSG), 10);
        last_key = mvwgetch(W(W_SYSMSG), 0, 0);

        if (last_key == K[K_WINDOW_EXIT]) {
            break;
        }
    } while (last_key != K[K_WINDOW_EXIT]);

    WIN(SYSMSG, state) = state_old;
    focus = focus_old;
    windows_fill(W_SYSMSG, 1);
}
