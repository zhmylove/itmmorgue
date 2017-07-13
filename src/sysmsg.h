// vim: sw=4 ts=4 et :
#ifndef SYSMSG_H
#define SYSMSG_H

#define SYSMSG_MAXLEN 256
#define CHAT_NICK_MAXLEN 32

char *sysmsg;

void c_sysmsg_init();
void c_sysmsg_open();
void draw_sysmsg();
void c_sysmsg_add(char *str);
void s_sysmsg_add(char **sysmsg, char *str);
void send_sysmsg(mqueue_t *queue, const char *msg);

#endif /* SYSMSG_H */
