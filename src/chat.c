// vim: sw=4 ts=4 et :
#include "itmmorgue.h"
#include "windows.h"

char **chat;
#define CHAT_MSG_BACKLOG 16
#define CHAT_MSG_MAXLEN 16

void chat_init() {
    if ((chat = malloc(sizeof(char *) * CHAT_MSG_BACKLOG)) == NULL) {
        panic("Unable to allocate chat memory!");
    }
    
    for (int i = 0; i < CHAT_MSG_BACKLOG; i++) {
        if ((chat[i] = malloc(CHAT_MSG_MAXLEN + 1)) == NULL) {
            panic("Unable to allocate chat memory!");
        }

        char buf[] = "This is 0 msg";
        buf[8] = '0' + i;
        strncpy(chat[i], buf, CHAT_MSG_MAXLEN);
        chat[i][CHAT_MSG_MAXLEN] = '\0';
    }
}

void draw_chat() {
    if (chat == NULL) {
        return;
    }

    struct timeval time;
    gettimeofday(&time, NULL);

    MVW(W_CHAT, 0, 0, "%d ", time.tv_sec);
    for (int i = 0; i < CHAT_MSG_BACKLOG; i++) {
        MVW(W_CHAT, i + 1, 0, "%s", chat[i]);
    }
}

