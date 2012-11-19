#include "mm.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
	status();

	int *a = (int*)my_malloc(sizeof(int));
	*a = 5;
	printf("%d\n",*a);
	status();

	char* b = (char*)my_malloc(100);
	status();
	b[0] = 'c';
	b[1] = 'o';
	b[2] = 'o';
	b[3] = 'l';
	b[4] = '\0';
	printf("%s\n",b);


	char* c = (char*)my_malloc(80);
	status();
	
	char* d = (char*)my_malloc(9978);
	d[9977] = '\0';

	status();
	my_free((void*)b);
	status();
	my_free((void*) d);
	status();
	char *e = (char*)my_malloc(80);
	status();
	my_free((void*)c);
	status();
	my_free((void*)a);
	status();
	my_free((void*)e);
	status();
	char buffer[3];
	printf("Press any key to continue...");
	fgets(buffer, 2, stdin);

}

