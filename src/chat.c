// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "windows.h"
#include "client.h"
#include "chat.h"
#include "keyboard.h"
#include "windows.h"

char *chat;
char input[CHAT_MSG_MAXLEN];
size_t inputpos;

void chat_init() {
    if ((chat = malloc(CHAT_MSG_BACKLOG * CHAT_MSG_MAXLEN + 1)) == NULL) {
        panic("Unable to allocate chat memory!");
    }
    
    int pos = 0;
    chat[0] = '\0';
    input[0] = '\0';

    for (int i = 0; i < CHAT_MSG_BACKLOG; i++) {
        char buf[] = "This is 0 msg";
        buf[8] = '0' + i;
        strcpy(chat + pos, buf);
        pos += strlen(chat + pos);
        chat[pos++] = '\n';
    }

    chat = "<kk> hi hi!\n"
        "adduxa joined #haskell\n"
        "<adduxa> hi!\n"
        "<kk> hi\n"
        "<adduxa> what's up? :)\n"
        "<kk> I am sitting at work, having nothing todo, and my stumach "
        "hurts like someone gave me H2SO4 for breakfest\n"
        "<kk> s/stumach/stomach/\n"
        "<adduxa> lydrogen sulfate?\n"
        "<kk> I already got something from a collegue, lets see if it helps\n"
        "<kk> Hmm, I should go home and continue reading eye of the world\n";
}

void draw_chat() {
    if (chat == NULL) {
        return;
    }

    int size = windows[W_CHAT].max_x * windows[W_CHAT].max_y;
    int len = strlen(chat);
    char *chatptr = chat + len - 2;

    if (size > len) {
        MVW(W_CHAT, 0, 0, "%s", chat);
    } else {
        while (size > 0) {
            switch (*chatptr--) {
                case '\n':
                    size -= windows[W_CHAT].max_x;
                    break;
                default:
                    size -= 1;
            }
        }
        while (*chatptr++ != '\n');
        MVW(W_CHAT, 0, 0, "%s", chatptr);
    }

    if (windows[W_CHAT].state == LARGE) {
        MVW(W_CHAT, windows[W_CHAT].max_y - 1, 0, "> %s", input);
    }
}

void chat_open() {
    int state_old = windows[W_CHAT].state;
    int focus_old = focus;

    windows[W_CHAT].state = LARGE;
    focus = W_CHAT;
    windows_fill(W_CHAT, 1);

    do {
        windows_redraw();

        wtimeout(W(W_CHAT), 100);
        if ((last_key = mvwgetch(W(W_CHAT), 0, 0)) == K[K_EXIT]) {
            break;
        } else {
            ungetch(last_key);
        }
        mvwgetnstr(W(W_CHAT), windows[W_CHAT].max_y - 1, 0, input + inputpos,
                sizeof(input) - inputpos);
    } while (last_key != K[K_EXIT]);

    windows[W_CHAT].state = state_old;
    focus = focus_old;
    windows_fill(W_CHAT, 1);
}
