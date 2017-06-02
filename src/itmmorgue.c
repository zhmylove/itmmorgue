#include <unistd.h>

int main(int argc, char *argv[]) {
	return ! write(1, "japh,", 5);
}
