#include <stdio.h>

int main(int argc, char* argv[])
{
	/*
	if (argc == 1) {
		printf("No arguments passed.\n");
		return 0;
	}
	*/

	for (int i = 0; i < argc; ++i) {
		printf("Argument %2d : %s\n", i, argv[i]);
	}
	return 0;
}
