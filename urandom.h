#ifndef URANDOM_H_
#define URANDOM_H_

#include <stdio.h>
#include <stdlib.h>
/* best random source on UNIX-like systems. */

int urandom() {
#ifdef __unix__
	int var;
	FILE *fd = fopen("/dev/urandom", "r");
	fread(&var, sizeof(int), 1, fd);
	fclose(fd);
	return var;
#else
	return 4;
#endif
}

#endif /* URANDOM_H_ */
