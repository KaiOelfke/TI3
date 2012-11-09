/*
	Aufgabe 1: Protection Rings

	Das Konzept der Protection Rings dient der Computersicherheit.
	Jeder Ring kann dabei nur auf seinen eigenen Bereich und den Bereich
	höherere Ringe zugreifen. Wenn also ein Programm im Ring 3 auf eine
	Ressource im Ring 0 zugreifen möchte, kann es dies nur mit einer
	definierten Schnittstelle und nicht direkt. Deswegen befindet sich 
	der Kernel auch immer im Ring 0 und die Anwendungen im Ring 3, damit
	Anwendungen nicht einfach Kernelfunktionen benutzen können um Schaden
	anzurichten. Heutzutage wird meistens nur der Ring 0 für den Kernel
	und Betriebssystemfunktionen genutzt und für andere Prozesse und
	Anwendungen der Ring 3. Protection Rings sind also eine Abstraktion
	für bessere Systemsicherheit und Stabilität.

	Aufgabe 2:

	Ein Microkernel beschränkt sich auf grundlegende Speicherverwaltung
	und Kommunikation zwischen Prozessen. 

	Ja, aber der Kernel verwaltet auch die Ausführung der Prozesse 
	(Scheduling). Prinzipiell beschränkt sich der Microkernel sonst,
	aber auf diese Funktionen.

	Im Kernelmodus muss das Betriebssystem darauf achten, dass die 
	Speicherbereiche verschiedener Prozesse voneinander isoliert sind. 

	Nein, weil der Kernel im Ring 0 ist und auf alles zugreifen darf. 
	Das Betriebssystem muss die Speicherbereiche verwalten können und 
	da im Kernelmodus auch keine Anwendung ausgeführt wird, kann keine
	Anwendung Schaden anrichten mit dem Zugriff auf Daten eines anderen
	Programms. Im Usermode muss das Betriebssystem dagegen die Speicher-
	bereiche isolieren, weil sonst ein Programm A ein anderes Programm B
	beschädigen könnte.

	Wenn Interrupt Service Routinen nebenläufig abgearbeitet werden, muss
	das Betriebssystem darauf achten, dass der Prozessor beim Timer-
	Interrupt nicht in den Benutzermodus wechselt. 

	Da z.B. Hardware ISR oft Ring 0 Funktionen benötigen, darf bei einem Timer
	Interrupt nicht einfach in den Benutzermodus gewechselt werden. Weil 
	dann z.B. ein Interrupt für die Tastatureingabe nicht fertig bearbeitet
	werden kann, wenn der Tastaturzugriff nur im Kernelmode möglich ist.

	Bei Microkerneln kommt es häufiger zu einem Kontextwechsel als bei
	monolithischen Kerneln

	Ja, weil bei einem monolithischen Kernel muss einmal der Kernelmode
	gestartet werden, die gewünschte Funktion ausgeführt werden und
	zurück gewechselt werden. Beim Microkernel dagegen wird in den Kernel
	mode gewechselt, dann ein Programm im Usermode für die Funktion gestartet,
	für das Ergebnis wird wieder in den Kernelmode gewechselt und dann wird
	vom Kernel das Ergebnis wieder zurück zu dem aufrufenden Programm über
	mittelt und deswegen wieder in den Usermode gewechselt.

	Aufgabe 3: System Calls

	Monolithischer Kernel
	1. Funktionsaufrufe werden eingefügt:
		- Programm muss immer in Ring 0 ausgeführt werden für Hardwarezugriff
		- keine Sicherheitsmechanismen
		- Funktionsadresse wird einfach eingefügt beim Kompilieren
		- Wird als normale Funktion ausgeführt, die sich nicht unterscheidet vom restl.
		Programm
		- Es gibt also keine Kontextwechsel
		- Monolithischen Kernel
	2. Supervisorcall:
		- Spezialereignis wie Interrupt, Exception, etc. wird ausgelöst durch Befehl
		- Eine Tabelle listet ISR auf und ISR wird im Kernelmode ausgeführt
		- Parameter durch Compiler eingfügt oder zwischengeschaltete Funktionen
		- Rücksprung zum Programm
		- nur Modewechsel, kein Prozesswechsel
	Microkernel:
	3. Aufruf von Systemmodul:
		- Programm erzeugt Ereignis (Usermode)
		- Mikrokernel reagiert auf Ereignis (Kernelmode)
		- Mikrokernel wechselt zum Systemmodulprozess (Prozesswechsel)
		- Systemmodul wird ausgeführt (Usermode)
		- Systemmodul terminiert (Usermode -> Kernelmode, Prozesswechsel)
		- Mikrokernel reagiert und wechselt zurück zum aufrufenden Programm 
		(Kernelmode -> Usermode, Prozesswechsel Fortsetzung)
	4. Dispatching
		- Programm erzeugt Ereignis (Usermode)
		- Microkernel nimmt dies zu Kenntnis (Kernelmode) 
		- Microkernel initialisiert Auftrag (Kernelmode)
		- Systemmodul akzeptiert Auftrag (Usermode)
		- Systemmodul alarmiert Kernel bei Fertigstellung (Usermode)
		- Kernel nimmt dies zu Kenntnis (Kernelmode)
		- Kernel benachrichtigt Programm (Kernelmode)
		- Programm nimmt Ergebnis entgegen (Usermode)


*/
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
