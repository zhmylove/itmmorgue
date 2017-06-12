#include <unistd.h>

int main(int argc, char *argv[]) {
	(void)argc, (void)argv;
	return 5 == write(1, "japh,", 5);
}
