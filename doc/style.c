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
    (void)fmt; // unused parameter
    return;
}

int main(int argc, char *argv[]) {
    if (argc <= 3) {                 // comment 
        return 1;                    // next-line
    }                                // next-line
    func(argv[0], argv[1], argv[2]); // end of comment

#define MACRO_USED_HERE(a) func(argv[0], (a))
    MACRO_USED_HERE(argv[1]);
#undef MACRO_USED_HERE

    if (argc == 2) return 2;

    for (int i = 0; i < 10; i++);

    return 0;
}
