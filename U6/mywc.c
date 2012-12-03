#include <stdio.h>
#define BUFFER_SIZE (64 * 1024)

int main(int argc, char *argv[])
{
	if (argc != 2 && argc != 1)
	{
		perror("Programm muss mit mywc file_name oder mywc stdin aufgerufen werden.");
		return 1;
	}
	FILE* file = NULL;
	if (argc == 2)
		file = fopen(argv[1],"r");
	int words = 0;
	int lines = 0;
	int bytes = 0;
	if (file || argc == 1)
	{
		int c;
		if (argc == 2)
			c = fgetc(file);
		else 
			c = getchar();
		int prevC = 32;
		while (c != EOF)
		{
			bytes++;
			if (((c > 8 && c < 14) || c == 32 || c == EOF) && !((prevC > 8 && prevC < 14) || prevC == 32 || prevC == EOF))
				words++;
			if (c == '\n')
				lines++;
			prevC = c;
			if (argc == 2)
				c = fgetc(file);
			else 
				c = getchar();
		}
		if (!((prevC > 8 && prevC < 14) || prevC == 32))
			words++; 
		printf("Words: %d Lines: %d Bytes: %d\n", words, lines, bytes);
		if (file) fclose(file);
		return 0;
	}
	else
	{
		perror("Konnte Datei nicht lesen.");
		fclose(file);
		return 1;
	}
}