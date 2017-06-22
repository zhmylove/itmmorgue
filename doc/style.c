// vim: et sw=4 ts=4 :

/*
 * Meanless function.
 *
 * fmt : format
 * ... : arguments
 *
 * ret : nothing
 */
void func(char *fmt, ...) {
    return;
}

int main(int argc, char *argv[]) {
    if (argc <= 3) {                 // comment 
        return 1;                    // next-line
    }                                // next-line
    func(argv[0], argv[1], argv[2]); // end of comment

    if (argc == 2) return 2;

    for (int i = 0; i < 10; i++);

    return 0;
}
