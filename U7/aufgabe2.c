#include <stdio.h>
#include <stdlib.h>
struct directoryEntry
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

int readAttributeByte(char byte)
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

int main(int argc, char *argv[]) {
	
	short int howmany;
	int rootDirectoryRegionStart;
	int dataRegionStart;
	int reservedRegionStart = 0;
	int fATRegionStart;
	int rootDirectoryRegionSize;
	int dataRegionSize;
	int reservedRegionSize;
	int fatRegionSize;
	// -----------
	short int bytesPerSector;
	short int reservedSectors;
	char numberOfFATs;
	char clusterInSector;
	short int sectorsPerFAT;
	short int rootEntiesCount;
	short int bootEntiesCount;
	int totalNumberOfSectors;
	int displayme;
	char string[11] ;
	FILE* imagefile = fopen(argv[1], "rb");	if (imagefile != NULL)
		{
			// Read BytesPerSector
			fseek(imagefile,0x0B, SEEK_CUR);
			fread(&bytesPerSector, 2, 1, imagefile);
			// Read ReservedSectors
			rewind(imagefile);
			fseek(imagefile,0x0E, SEEK_CUR);
			fread(&reservedSectors, 2, 1, imagefile);
			// Read Number of FAT copies
			rewind(imagefile);
			fseek(imagefile,0x10, SEEK_CUR);
			fread(&numberOfFATs, 1, 1, imagefile);
			// Read Sectors Per FAT
			rewind(imagefile);
			fseek(imagefile,0x016, SEEK_CUR);
			fread(&sectorsPerFAT, 1, 2, imagefile);
			// Read Root Enties Count
			rewind(imagefile);
			fseek(imagefile,0x011, SEEK_CUR);
			fread(&rootEntiesCount, 1, 2, imagefile);
			// Read Reserved Sectors
			rewind(imagefile);
			fseek(imagefile,0x0D, SEEK_CUR);
			fread(&clusterInSector, 1, 1, imagefile);
			// Read Large number of sectors
			rewind(imagefile);
			fseek(imagefile,0x020, SEEK_CUR);
			fread(&totalNumberOfSectors, 1, 4, imagefile);
			// file size
			rewind(imagefile);
			fseek(imagefile,0x1051B, SEEK_CUR);
			fread(&displayme, 1, 4, imagefile);
			//
			 reservedRegionStart = 0;
			 fATRegionStart = reservedRegionStart + reservedSectors;
			 rootDirectoryRegionStart = fATRegionStart +(numberOfFATs*sectorsPerFAT);
			
			 dataRegionStart = rootDirectoryRegionStart + ((rootEntiesCount*32)/bytesPerSector);
			 
			 reservedRegionSize = reservedSectors;
			 fatRegionSize = numberOfFATs * sectorsPerFAT;
			 rootDirectoryRegionSize = (rootEntiesCount*32)/bytesPerSector;
			 dataRegionSize = totalNumberOfSectors - (reservedRegionStart+fatRegionSize+rootDirectoryRegionSize);
			for (int i = 0; i < 27; i++)
			{
				fseek(imagefile, rootDirectoryRegionStart * bytesPerSector + 32 * i, SEEK_SET);
							DIRENTRY* rootDir = (DIRENTRY *) malloc(sizeof(DIRENTRY));
							fread(rootDir, sizeof(DIRENTRY), 1, imagefile);
							if (readAttributeByte(rootDir->attributeByte))
							{
								printf("%s\n",rootDir->filename);
								printf("%s\n",rootDir->extension);
							}
			}
			

			
			printf("ReservedSectors: %hd \n", reservedSectors);
			printf("ReservedRegion start :%d  %d\n",reservedRegionStart,displayme );
			printf("sectorsPerFAT : %hd\n", sectorsPerFAT);
			printf("bytesPerSector : %hd\n", bytesPerSector);
			printf("clusterInSector : %d\n", (int)clusterInSector);
			printf("fATRegionStart  :%d\n",fATRegionStart);
			printf("rootDirectoryRegionStart  :%d \n",rootDirectoryRegionStart);
			printf("dataRegionStart  :%d \n",dataRegionStart);
			printf("reservedRegionSize  :%d \n",reservedRegionSize);
			printf("fatRegionSize  :%d \n",fatRegionSize);
			printf("rootDirectoryRegionSize  :%d \n",rootDirectoryRegionSize);
			printf("dataRegionSize  :%d \n",dataRegionSize);
		}

	fclose(imagefile);
}