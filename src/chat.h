// vim: sw=4 ts=4 et :
#ifndef CHAT_H
#define CHAT_H

// #define CHAT_MSG_BACKLOG 16
#define CHAT_MSG_MAXLEN 256
#define CHAT_NICK_MAXLEN PLAYER_NAME_MAXLEN

char *chat;
char *schat;
char nickname[CHAT_NICK_MAXLEN + 1];
size_t chat_num_lines;

void c_chat_init();
void c_chat_open();
void draw_chat();
void c_chat_add(char *str);
void s_chat_add(char **chat, char *str);

#endif /* CHAT_H */
