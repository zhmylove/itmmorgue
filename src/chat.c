// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "windows.h"
#include "client.h"
#include "chat.h"
#include "keyboard.h"
#include "windows.h"

char input[CHAT_MSG_MAXLEN + 2];
size_t inputpos;

void c_chat_init() {
    if ((chat = malloc(CHAT_MSG_MAXLEN + 1)) == NULL) {
        panic("Unable to allocate chat memory!");
    }
    
    chat[0] = '\0';
    input[0] = '\0';
    strncpy(nickname, CONF_SVAL("player_nickname"), sizeof(nickname));

    mbuf_t mbuf;
    mbuf.msg.type = MSG_GET_CHAT;
    mbuf.msg.size = 0;
    mqueue_put(&c2s_queue, mbuf);
}

void draw_chat() {
    if (chat == NULL) {
        return;
    }

    int square = windows[W_CHAT].max_x * windows[W_CHAT].max_y;
    size_t len = strlen(chat);
    char *chatptr = chat + len - 2;

    if (windows[W_CHAT].state == LARGE) {
        MVW(W_CHAT, windows[W_CHAT].max_y - 1, 0, "> %s", input);
    }

    if (len < 2) {
        return;
    }

    if (windows[W_CHAT].state == LARGE && square > windows[W_CHAT].max_x) {
        square -= windows[W_CHAT].max_x;
    }

    int curr = 0;
    while (chatptr > chat && curr < square) {
        switch (*chatptr) {
            case '\n':
                curr += windows[W_CHAT].max_x - curr % windows[W_CHAT].max_x;
                break;
            default:
                if ((*chatptr & 0xC0) != 0x80) {
                    curr++;
                }
        }
        chatptr--;
    }

    if (chatptr != chat && *(chatptr - 1) != '\n') {
        while (*chatptr++ != '\n');
    }

    MVW(W_CHAT, 0, 0, "%s", chatptr);
}

void s_chat_add(char **schat, char *str) {
    int oldsize = strlen(*schat) + 1;
    int newsize = oldsize + strlen(str) + 1;

    // TODO take care of CHAT_MSG_BACKLOG
    // TODO do not forget that this routine is used by both client and server

    if ((*schat = realloc(*schat, newsize)) == NULL) {
        panic("Error reallocating server chat buffer!");
    }

    strcat(*schat, str);
}

void c_chat_add(char *str) {
    int oldsize = strlen(chat) + 1;
    int newsize = oldsize + strlen(str) + 1;

    // TODO take care of CHAT_MSG_BACKLOG
    // TODO do not forget that this routine is used by both client and server

    if ((chat = realloc(chat, newsize)) == NULL) {
        panic("Error reallocating chat buffer!");
    }

    strcat(chat, str);
}

void c_chat_open() {
    int state_old = windows[W_CHAT].state;
    int focus_old = focus;

    windows[W_CHAT].state = LARGE;
    focus = W_CHAT;
    windows_fill(W_CHAT, 1);

    do {
        windows_redraw();

        wtimeout(W(W_CHAT), 10);
        last_key = mvwgetch(W(W_CHAT), 0, 0);

        if (last_key == K[K_WINDOW_EXIT]) {
            break;
        } else if (last_key == K[K_BACKSPACE]) { // Backspace
            if (inputpos == 0) {
                continue;
            }

            inputpos--;
            while ((*(input + inputpos) & 0xC0) == 0x80) inputpos--;
            input[inputpos] = '\0';
        } else if (last_key == K[K_CHAT_SEND]) {
            if (inputpos == 0) {
                continue;
            }

            input[inputpos++] = '\n';
            input[inputpos++] = '\0';

            mbuf_t mbuf;
            size_t bufsize = strlen(input) + strlen(nickname) + 4;
            char *buf = malloc(bufsize);
            snprintf(buf, bufsize, "<%s> %s", nickname, input);
            size_t size = strlen(buf) + 1;
            mbuf.payload = malloc(size);
            mbuf.msg.type = MSG_NEW_CHAT;
            mbuf.msg.size = size;
            memcpy(mbuf.payload, buf, size);

            loggerf("[C] sending NEW_CHAT: [%s]", (char *)mbuf.payload);

            mqueue_put(&c2s_queue, mbuf);

            free(buf);

            inputpos = 0;
            memset(input, '\0', CHAT_MSG_MAXLEN + 2);
        } else {
            ungetch(last_key);
        }

        char buf[sizeof(input) - inputpos - 2];
        buf[0] = '\0';
        mvwgetnstr(W(W_CHAT), 0, 0, buf,
                sizeof(input) - inputpos - strlen(nickname));
        if (strnlen(buf, sizeof(buf) > 0)) {
            strncat(input + inputpos, buf, sizeof(input) - inputpos);
            inputpos = strlen(input);
        }
    } while (last_key != K[K_WINDOW_EXIT]);

    windows[W_CHAT].state = state_old;
    focus = focus_old;
    windows_fill(W_CHAT, 1);
}
