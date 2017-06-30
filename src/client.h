// vim: sw=4 ts=4 et :
#ifndef CLIENT_H
#define CLIENT_H

int max_x, max_y;
WINDOW *w_area;


// Dark and light colors for w_color()
enum colors {
    DWHITE, DYELLOW, DRED, DGREEN, DBLUE, DBLACK, DCYAN, DMAGENTA,
    LWHITE, LYELLOW, LRED, LGREEN, LBLUE, LBLACK, LCYAN, LMAGENTA
};

void init_screen();
void resize();
void sigwinch(int signum);
int client();

#endif /* CLIENT_H */
