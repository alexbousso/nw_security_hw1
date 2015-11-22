#include <stdio.h>
#include <stdlib.h>


void add(int x, int y, int *ret);
void mult(int x, int y, int *ret);

void divide(int x, int y, int *ret) {
	if(y==0){
		printf("Can't divide by zero!\n");
		*ret = -1;
		return;
	}
	*ret = x/y;
}

void incr_in_ptr(char* p) {
	(*p)++;
	return;
}

void doNothingAndStallTime(){
}

void recursion(int counter){
	if(counter<1){
		printf("finished a long recursion\n");
		return;
	}
	recursion(counter-1);
}

void main() {
	int x;
	int y;
	int *foo;
	int i;
	char p;
	char arr[4];
	int z;
	
	foo = (int *) malloc(sizeof(int));

	z=0;
	x=1;
	y=3;
	add(x,y,foo);
	printf("%d + %d is %d\n", x, y, *foo);
	mult(6,4,foo);
	printf("%d * %d is %d\n", 6,4,*foo);
	doNothingAndStallTime();
	doNothingAndStallTime();
	doNothingAndStallTime();
	doNothingAndStallTime();
	doNothingAndStallTime();
	doNothingAndStallTime();
	recursion(10);
	divide(6,0,foo);
	printf("%d / %d is %d\n", 6,0,*foo);
	divide(6,2,foo);
	printf("%d / %d is %d\n", 6,2,*foo);
	
	
	p = 52;
	printf("p before the change was %d\n", p);
	incr_in_ptr(&p);
	printf("p after the change is %d\n", p);
	
	printf("now enter your nickname\n");
	scanf("%s", arr);
	
	printf("hello %s! from now your name shall be ", arr);
	for(i=0; i<3; i++){
		incr_in_ptr(arr+i);
	}
	printf("%s. goodbye for now!\n", arr);
	printf("WAIT! z should be 0, and it's %d!\n", z);

	// if we free foo there is a segmentation fault :(
	//free(foo);
	return;

}


void add(int x, int y, int *ret) {
	int result;
	result = x+y;
	*ret = result;
	return;
}

void mult(int x, int y, int *ret) {
	*ret = x*y;
}