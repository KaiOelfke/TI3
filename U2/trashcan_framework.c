/* trashcan.c */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE			1024
#define PATHSIZE		256
#define TRASHFOLDER	".ti3_trash"	


char copy_buffer[BUFSIZE];


/* 'copy' kopiert den Dateiinhalt einer Datei namens 
 * "source". Eine Kopie target wird nur erzeugt, wenn 
 * eine Datei "target" noch nicht existiert.
 * Die Zugriffsrechte werden nicht kopiert, sondern
 * auf "rwx --- ---" gesetzt.
 */

char * createPath(char *folderName, char *fileName)
{
	int pathLength = strlen(folderName) + strlen(fileName) + 2;
	char *path = (char *) malloc(pathLength * sizeof(char));
	if (path == NULL)
		return NULL;
	strcpy(path, folderName);
	char slash[2] = "/\0";
	strcat(path, slash);
	strcat(path, fileName);
	return path;
} 
int copy(char *source, char *target)
{
	int fdOriginal = open(source, O_RDONLY);
	if (fdOriginal < 0)
		return -1;
	int fdCopy = open(target, O_WRONLY | O_CREAT | O_EXCL | O_EXLOCK, S_IRWXU);
	if (fdCopy < 0)
		return -2;
	char buffer[BUFSIZE];
	for (;;) 
	{
		int bytesRead = read(fdOriginal, buffer, BUFSIZE);
		if (bytesRead == 0) 
			break;
		if (bytesRead == -1)
			return -2;
		char *writePostion = buffer;
		while (bytesRead > 0)
		{
			int bytesWritten = write(fdCopy, buffer, bytesRead);
			if (bytesWritten == -1)
				return -2;
			bytesRead -= bytesWritten;
			writePostion += bytesWritten;
		}
	}
 	int closeOriginal = close(fdOriginal);
 	int closeCopy = close(fdCopy);
 	if (closeOriginal == -1 || closeCopy == -1)
 		return -4;
	return EXIT_SUCCESS;
}


char parse_command(char *command)
{
	return command[1];
}


/* erzeugt einen Ordner foldername */
int setup_trashcan(char *foldername)
{
	struct stat sb;
	if (stat(foldername, &sb) == 0 && S_ISDIR(sb.st_mode))
		return 0;
	else 
		return mkdir(foldername, S_IRWXU);
}

/* führt trashcan -p[ut] filename aus */
int put_file(char *foldername, char *filename)
{
	char *path = createPath(foldername,filename);
	if (path == NULL)
		return -5;
	int copyError = copy(filename,path);
	free(path);
	if (copyError == -1)
		return -1;
	else if (copyError == -2) 
		return -2;
	else if (copyError == -4)
		return -4;
	int removeError = unlink(filename);
	if (removeError != 0)
		return -3;
	return 0;
}


/* führt trashcan -g[et] filename aus */
int get_file(char *foldername, char *filename)
{
	char *path = createPath(foldername,filename);
	if (path == NULL)
		return -5;
	int copyError = copy(path, filename);
	if (copyError == -1)
		return -1;
	else if (copyError == -2) 
		return -2;
	else if (copyError == -4)
		return -4;
	int removeError = unlink(path);
	free(path);
	if (removeError != 0)
		return -3;
	return 0;
}

/* führt trashcan -r[emove] filename aus */
int remove_file(char *foldername, char *filename)
{
	char *path = createPath(foldername,filename);
	if (path == NULL)
		return -5;
	int removeError = unlink(path);
	free(path);
	if (removeError != 0)
		return -3;
	return 0;
}


int main(int argc, char *argv[])
{
	char trashFolder[11] = TRASHFOLDER;
	if (argc != 3) {
		printf("Benutzung: -[p,g,r] filename!\n p = put in trash, g = restore from trash, r = remove from trash.\n");
		return EXIT_FAILURE;
	} else {
		int setupError = setup_trashcan(trashFolder);
		if (setupError != 0)
		{
			printf("Setup failed: Working folder could not be created.\n");
			return EXIT_FAILURE;
		}
		char command = parse_command(argv[1]);
		switch (command) {
			case 'p':
				if (argc != 3) {
					printf("...not enough arguments!\n");
					return EXIT_FAILURE;
				}
				int p_error = put_file(trashFolder, argv[2]);
				switch (p_error) {
					case -1:
						printf("...source file not found!\n");
						break;
					case -2:
						printf("...trash file was not created!\n");
						break;
					case -3:
						printf("...source file could not be removed!\n");
					case -4:
						printf("Dateien konnten nicht korrekt geschlossen werden.\n");
					case -5:
						printf("Es konnte nicht ausreichend viel Speicher reserviert werden.\n");
					default:
						break;
				}
				break;
			case 'g':
				if (argc != 3) {
					printf("...not enough arguments!\n");
					return EXIT_FAILURE;
				}
				int g_error = get_file(trashFolder, argv[2]);
				switch (g_error) {
					case -1:
						printf("...trash file not found!\n");
						break;
					case -2:
						printf("...restore file was not created!\n");
						break;
					case -3:
						printf("...trash file could not be removed!\n");
					case -4:
						printf("Dateien konnten nicht korrekt geschlossen werden.\n");
					case -5:
						printf("Es konnte nicht ausreichend viel Speicher reserviert werden.\n");
					default:
						break;
				}
				break;
			case 'r':
				if (argc != 3) {
					printf("...not enough arguments!\n");
					return EXIT_FAILURE;
				}
				int r_error = remove_file(trashFolder, argv[2]);
				if (r_error == -3)
					printf("...trash file could not be removed!\n");
				if (r_error == -5)
					printf("Es konnte nicht ausreichend viel Speicher reserviert werden.\n");
				break;
			default :
				printf("...unknown command!\n");
				return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
