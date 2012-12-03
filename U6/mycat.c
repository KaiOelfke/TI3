#include <stdio.h>
#define BUFFER_SIZE (64 * 1024)

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		perror("Programm muss mit mycat file_name aufgerufen werden.");
		return 1;
	}
	FILE* file = fopen(argv[1],"r");
	if (file)
	{
		char buffer[BUFFER_SIZE];
		while (fgets(buffer, BUFFER_SIZE, file) != NULL)
			fputs(buffer, stdout);
		fclose(file);
		return 0;
	}
	else
	{
		perror("Konnte Datei nicht lesen.");
		fclose(file);
		return 1;
	}
}
