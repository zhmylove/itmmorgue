// vim: sw=4 ts=4 et :
#ifndef SYSMSG_H
#define SYSMSG_H

#define SYSMSG_MAXLEN 256

char *sysmsg;

void c_sysmsg_init();
void c_sysmsg_open();
void draw_sysmsg();
void c_sysmsg_add(char *str);
void s_sysmsg_add(char **sysmsg, char *str);
void send_sysmsg(connection_t *connection, enum msg_sysmsg_type type,
        const char *msg);
void close_connection(connection_t *connection);

#endif /* SYSMSG_H */
