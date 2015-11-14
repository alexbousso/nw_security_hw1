#include <stdio.h>
#include <stdlib.h>

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

#include <stdio.h>
#include <assert.h>

void foo(int i);
void bar(long i);
void goo(int i);
void problem(int *arr);

void main() {
	int __stack_canary_protection = urandom();
	int canary1 = __stack_canary_protection;
	long i = 0;
	int a = 42;
	int arr[2];
	int canary2 = __stack_canary_protection;
	bar(i);

	goo(a);
	assert(a == 42);

	printf("%s\n", "All good!");

	problem(arr);

	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
	return;
	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
}

void foo(int i) {
	int __stack_canary_protection = urandom();
	int canary1 = __stack_canary_protection;
	char ch = 42;
	int j = 42;
	int canary2 = __stack_canary_protection;

	if (j == i) {
	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
		return;
	}
	else {
		printf("a");
	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
		return;
	}

	while (42 != ch) {
		printf("a");
	}
	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
}

void bar(long i) {
	int __stack_canary_protection = urandom();
	int canary1 = __stack_canary_protection;
	int canary2 = __stack_canary_protection;
	assert(i == 0);
	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
	return;
	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
}

void goo(int i) {
	int __stack_canary_protection = urandom();
	int canary1 = __stack_canary_protection;
	int j = 7;
	int canary2 = __stack_canary_protection;

	j = 42;
	assert(j == i);

	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
}

void problem(int *arr) {
	int __stack_canary_protection = urandom();
	int canary1 = __stack_canary_protection;
	int i = 0;
	int canary2 = __stack_canary_protection;
	for (i = 0; i < 4; i++) {
		arr[i] = 42;
	}
	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
	return;
	if (canary1 != __stack_canary_protection || canary2 != __stack_canary_protection) {
		printf("Alert! Buffer Overflow detected.");
		exit(1);
	}
}
