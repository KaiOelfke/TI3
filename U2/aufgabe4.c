#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#define BUFFER_SIZE (64 * 1024)
int copy(char *source, char *target)
{
	int fdOriginal = open(source, O_RDONLY);
	if (fdOriginal < 0)
	{
		perror("Konnte Quelle nicht oeffnen:");
		return EXIT_FAILURE;
	}
	int fdCopy = open(target, O_WRONLY | O_CREAT | O_EXCL | O_EXLOCK, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH);
	if (fdCopy < 0)
	{
        perror( "Konnte Zieldatei nicht erstellen:");
		return EXIT_FAILURE;
	}
	char buffer[BUFFER_SIZE];
	for (;;) 
	{
		int bytesRead = read(fdOriginal, buffer, BUFFER_SIZE);
		if (bytesRead == 0) break;
		if (bytesRead == -1)
		{
			perror("Fehler beim Lesen:");
			return EXIT_FAILURE;
		}
		char *writePostion = buffer;
		while (bytesRead > 0)
		{
			int bytesWritten = write(fdCopy, buffer, bytesRead);
			if (bytesWritten == -1)
			{
				perror("Fehler beim Schreiben");
				return EXIT_FAILURE;
			}
			bytesRead -= bytesWritten;
			writePostion += bytesWritten;
		}
	}
 	int closeOriginal = close(fdOriginal);
 	int closeCopy = close(fdCopy);
 	if (closeOriginal == -1 || closeCopy == -1)
 	{
 		perror("Fehler beim Schließen");
 		return EXIT_FAILURE;
 	}
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		perror("Bitte Programm mit: Quelldatei Zieldatei aufrufen\n");
		return EXIT_FAILURE;
	}
	int result = copy(argv[1],argv[2]);
	if (result == -1) return EXIT_FAILURE;
	return EXIT_SUCCESS;
}