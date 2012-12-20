/*

	-- Aufgabe 1

		Zwei Protokolle sind beteiligt. 
			- Das erste Protokoll ist DNS:
				DNS-Protokoll kümmert sich darum, dass die Domain zur IP-Adresse
				umgewendelt wird.
				z.B:
					www.fu-berlin.de -> 160.45.170.10
				DNS Protokoll basiert auf UDP.

			- Das zweite Protokoll ist Http:

				Http-Protokoll ist eine Anwendung für TCP-Protokoll.
				Die Kommunikationseinheiten in HTTP zwischen Bernutzer und Server werden 
				als Nachrichten bezeichnet.
				die Nachrichten  besteht auf zwei Teile. Der erste Teil ist Kopf "Header".
				in dem alle Informationen über die übergetragene Daten gespeichert wird.
				z.B ob die Daten ein Html-Text oder ein Media "Video" sind.
				Der zweite Teil ist die Daten. Die Daten werden zu Segmente zerlegt. 
				und dann mit TCP Protokoll zu dem Benutzer geschickt wird.
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
struct __attribute__ ((__packed__)) bootSector
{
	char code[3];
	char nameOfOS[8];
	short int bytesPerSector; //Usally 512 bytes
	char sectorsPerCluster;
	short int reservedSectors;
	char numberOfFATs; //Usally 2
	short int maxRootEntries; //512 recommended
	short int smallNumberOfSectors;
	char mediaDescriptor;
	short int sectorsPerFAT;
	short int sectorsPerTrack;
	short int numberOfHeads;
	int hiddenSectors;
	int largeNumberOfSectors;
	char driveNumber;
	char reservedForWindowsNT;
	char extendedBootSignature;
	int volumeSerialNumber;
	char volumeLabel[11];
	char fileSystemType[8];
	char bootstrapCode[448];
	short int bootSectorSignature;
};

typedef struct bootSector BOOTSECTOR;

struct __attribute__ ((__packed__)) directoryEntry
{
	char filename[8];
	char extension[3];
	char attributeByte;
	char reservedWindowsNT;
	char creationMSStamp;
	short int creationTime;
	short int creationDate;
	short int lastAccessDate;
	short int reservedFAT32;
	short int lastWriteTime;
	short int lastWriteDate;
	short int startingCluster;
	int fileSizeInBytes;
};

typedef struct directoryEntry DIRENTRY;

void longToShortFileName(char * filename){
	
	char* position = NULL;
	int charCount = 0;
	int i = 0;
	int len = strlen(filename);
	while (charCount < 9 || i < len) {
			
		if (filename[i] != ' ')
			charCount++;
		i++;
	}
	filename[i] = '\0';
	
	position = strchr(filename, ' ');
	
	while (position != NULL)
	{
		strcpy(position, position + 1);
		position = strchr(filename, ' ');
	}
	
	int fileNameLength = strlen(filename);
	if (fileNameLength >8){
		filename[6] = '~';
		filename[7] = '1';
		filename[8] = '\0';
		
	}	
}

void splitPath(char* folder, char* path)
{
	char* firstSeperator = strchr(path, '/');
	if (firstSeperator == NULL)
	{
		strcpy(folder, path);
		path[0] = '\0';
		return;
	}
	char* newPath = firstSeperator + 1;
	int folderLength = strlen(path) - strlen(newPath) - 1;
	strncpy(folder, path, folderLength);
	folder[folderLength] = '\0';
	longToShortFileName(folder);
	strcpy(path,newPath);
}

char isFolder(char byte)
{
	return byte & 16;
}
char readAttributeByte(char byte)
{
	byte = byte >> 1;
	for (int i = 0; i < 3; i++)
	{
		if ((byte & 1) == 1)
			return 0;
		else
			byte = byte >> 1;
	}
	return 1;
}
int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		perror("Programm muss mit <./myls image directory> aufgerufen werden.");
		return EXIT_FAILURE;
	}
	FILE* imageFile = fopen(argv[1], "r");
	if (imageFile == NULL)
	{
		perror("Konnte image nicht oeffnen.");
		return EXIT_FAILURE;
	}
	BOOTSECTOR bootSector;
	fread(&bootSector, sizeof(BOOTSECTOR), 1, imageFile);
	
	int startReservedRegion = 0;
	int startFATRegion = startReservedRegion + bootSector.reservedSectors;
	int startRootDirectoryRegion = startFATRegion + bootSector.numberOfFATs * bootSector.sectorsPerFAT;
	
	int startDataRegion = startRootDirectoryRegion + (bootSector.maxRootEntries * 32) / bootSector.bytesPerSector;
	 
	int sizeReservedRegion = bootSector.reservedSectors;
	int sizeFATRegion = bootSector.numberOfFATs * bootSector.sectorsPerFAT;
	int sizeRootDirectoryRegion = (int) ceil((bootSector.maxRootEntries * 32) / bootSector.bytesPerSector);
	int totalNumberOfSectors = bootSector.smallNumberOfSectors == 0 ? bootSector.largeNumberOfSectors : bootSector.smallNumberOfSectors;
	int sizeDataRegion = totalNumberOfSectors - (startReservedRegion + sizeFATRegion + sizeRootDirectoryRegion);
	

	DIRENTRY entry;
	char* path = argv[2];
	char folder[strlen(path)];
	splitPath(folder,path);
	char notFinished = 1;
	int offset = startRootDirectoryRegion * bootSector.bytesPerSector;
	fseek(imageFile, offset, SEEK_SET);
	int i = 0;
	while (fread(&entry, sizeof(DIRENTRY), 1, imageFile) == 1 && notFinished)
	{
		switch ((unsigned char) entry.filename[0]) //unsigned char cast, weil sonst 229 zu gross ist. gcc warnung
		{
			case '\0':
				notFinished = 0;
				break;
			case 229:
				break;
			default:
				if (folder[0] == '\0' && readAttributeByte(entry.attributeByte))
					printf("%s\n",entry.filename);
				else if (readAttributeByte(entry.attributeByte) && isFolder(entry.attributeByte) && strncmp(folder,entry.filename,strlen(folder)) == 0) 
				{
					offset = (startDataRegion + (entry.startingCluster - 2) * bootSector.sectorsPerCluster) * bootSector.bytesPerSector;
					splitPath(folder,path);
					i = -1;
					break;
				}
				break;
		} 
		i++;
		fseek(imageFile, offset + 32 * i, SEEK_SET);
	}
	fclose(imageFile);
	return EXIT_SUCCESS;
}
