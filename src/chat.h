// vim: sw=4 ts=4 et :
#ifndef CHAT_H
#define CHAT_H

#define CHAT_MSG_BACKLOG 16
#define CHAT_MSG_MAXLEN 256

void c_chat_init();
void c_chat_open();
void draw_chat();
void c_chat_add(char *str);

#endif /* CHAT_H */
