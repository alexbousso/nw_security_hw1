#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "test_utils.h"

int main(const int argc, const char *argv[]) {
	 if (argc != 2) {
		 ERROR("Usage %s <program.c>\n", argv[0]);
		 exit(1);
	 }

	const char *filename = argv[1];

	CreateProtectedFile(filename);

	return 0;
}
