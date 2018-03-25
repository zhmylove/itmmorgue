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
    // This function is full of dark magic. Do hesitate even read it.
    if (sysmsg == NULL) {
        return;
    }

    int square = WIN(SYSMSG, max_x) * WIN(SYSMSG, max_y);
    size_t len = strlen(sysmsg);
    char *sysmsgptr = sysmsg + len - 1;

    if (len < 2) {
        return;
    }

    // skip 'square' characters
    while (sysmsgptr > sysmsg && square > 0) {
        int msglen = 0;
        char *msgptr = sysmsgptr;
        while (--msgptr > sysmsg && *msgptr != '\n');
        char *nextptr = msgptr++;
        while (msgptr < sysmsgptr) {
            if ((*++msgptr & 0xC0) != 0x80) {
                msglen++;
            }
        }
        square -= WIN(SYSMSG, max_x) - msglen % WIN(SYSMSG, max_x);
        if (msglen > square) {
            while (square) {
                if ((*--sysmsgptr & 0xC0) != 0x80) {
                    --square;
                }
            }
            break;
        }
        square -= msglen;
        sysmsgptr = nextptr;
    }

    if (*sysmsgptr == '\n') {
        sysmsgptr++;
    }

    MVW(W_SYSMSG, 0, 0, "%s", sysmsgptr);
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
