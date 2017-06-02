#include <unistd.h>

int main(int argc, char *argv[]) {
	(void)argc, (void)argv;
	return ! write(1, "japh,", 5);
}
