#include "mm.h"
#include <stdio.h>
#include <stdlib.h>
//Groesse des Speichers, den die Speicherverwaltung verwalten soll
#define memorySize 10306

//Zustand von einem Speicherblock
enum memBlockState{not_allocated=0, allocated=1};
//zur spaeteren Ausgabe..
char boolStr[2][6] = {{"FALSE"},{"TRUE"}};

//ein Speicherblock
typedef struct _memoryblock {
	void* data;			//Zeiger auf die Daten
	int dataLength;			//Laenge des Blocks
	enum memBlockState state;	//Wird der Block benutzt?
	struct _memoryblock * nextBlock; //Zeiger auf naechsten Speicherblock
} memoryBlock;

#define memoryBlockHeaderSize sizeof(memoryBlock)

//definition von "privaten" funktionen
void mergeFreeBlocks();
memoryBlock* splitBlock(memoryBlock* block, int byteCount);

//Erster Speicherblock in der Liste
memoryBlock* head;
//Unser Speicher, den wir fuer die Daten und deren Verwaltung benutzen koennen
char memory[memorySize];

//Ist unsere Speicherverwaltung initialisiert und bereit benutzt zu werden?
int b_initialized = 0;

//Initialisiere die Datenstruktur(en) fuer unsere Speicherverwaltung
void initialize()
{
	if(!b_initialized)
	{
		//initialisiere head in unserem Speicher <memory>
		b_initialized = 1;
		//TODO INITIALISIERUNG	
		head = (memoryBlock *) memory;
		head->data = (void *)memory + memoryBlockHeaderSize;
		head->dataLength = memoryBlockHeaderSize;
		head->state = allocated;
		head->nextBlock = NULL;
		memoryBlock *new = (memoryBlock *) ((void *)head + head->dataLength);
		head->nextBlock = new;
		new->dataLength = memorySize - head->dataLength;
		new->state = not_allocated;
		new->data = (void *) new + memoryBlockHeaderSize;
		new->nextBlock = NULL;
	}
}

//liefert den ingesamt freien Speicherplatz zurueck
int get_free_space()
{
	if(!b_initialized)
		return -1;
	int count = 0;
	memoryBlock *block = head;
	while(block != NULL)
	{
		if(block->state == not_allocated)
			count = count + block->dataLength - memoryBlockHeaderSize;
		block = block->nextBlock;
	}
	return(count);
}

//versucht einen Speicherbereich mit <byteCount> vielen Bytes zu reservieren
//Falls dies nicht gelingt wird ein NULL (0) Pointer zurueckgeliefert
void* my_malloc(int byteCount)
{
	if(!b_initialized)
	{
		initialize();
	}
	//Wenn der insgesamt verfuegbare Speicherplatz kleiner ist
	//als der angeforderte, koennen wir gleich aufhoeren!
	if(byteCount > get_free_space())
	{
		return(NULL);
	}
	//TODO
	//SUCHE NACH EINEM GEEIGNETEN FREIEN SPEICHERBLOCK, MIT MEHR ALS <byteCount>
	//VIELEN BYTES
	memoryBlock *block = head;
	while (block->nextBlock != NULL && (block->nextBlock->dataLength < byteCount + memoryBlockHeaderSize * 2 || block->nextBlock->state == allocated))
		block = block->nextBlock;
	if (block->nextBlock == NULL)
	{
		/*printf("\nok91\n");
		block->nextBlock = (memoryBlock *) ((void *)block + block->dataLength);
		block->nextBlock->dataLength = memoryBlockHeaderSize + byteCount;
		block->nextBlock->nextBlock = NULL;
		block->nextBlock->state = allocated;
		block->nextBlock->data = (void *) block->nextBlock + memoryBlockHeaderSize;
		printf("\nok97\n");*/
		printf("Fehler. Nicht ausreichend Speicher oder zu sehr fragmentiert.\n");
		exit(EXIT_FAILURE);

	}
	//UNTERTEILUNG DIESES BLOCKS, SO DASS NICHT UNN�TIG VIEL SPEICHERPLATZ VERBRAUCHT WIRD
	else 
		splitBlock(block->nextBlock, byteCount);
	//R�CKGABE DES ZEIGERS AUF DEN ZU BENUTZENDEN SPEICHERBEREICH
	return (void *) block->nextBlock->data;
}

//Sofern moeglich teilt die Funktion splitBlock einen Block in 2 Bloecke,
//so dass einer der beiden Bloecke <byteCount> viel Speicherplatz enthaelt.
//Die Funktion liefert denjenigen Block zurueck, der <byteCount> viel Speicherplatz
//bereitstellt und passt die verkette Liste an.
memoryBlock* splitBlock(memoryBlock* block, int byteCount)
{
	//TODO 
	//IMPLEMENTIEREN //Vergleich eig unnoetig
	if (byteCount + memoryBlockHeaderSize * 2 > block->dataLength)
	{
		printf("Fehler. Block kann nicht geteilt werden, weil zu klein.\n");
		exit(EXIT_FAILURE);
	}
	memoryBlock *secondBlock = (memoryBlock *)((void *)block + memoryBlockHeaderSize + byteCount);
	secondBlock->dataLength = 1;
	secondBlock->dataLength = block->dataLength - byteCount - memoryBlockHeaderSize;
	secondBlock->data = (void *) secondBlock + memoryBlockHeaderSize;
	secondBlock->state = not_allocated;
	secondBlock->nextBlock = block->nextBlock;
	block->dataLength = byteCount + memoryBlockHeaderSize;
	block->data = (void *) block + memoryBlockHeaderSize; //Eig ueberflussig.
	block->state = allocated;
	block->nextBlock = secondBlock;
	return block;
}


//my_free versucht den Speicherbereich, auf den <p> verweist, wieder freizugeben.
//Sofern <p> in unserem Speicher gefunden wird, muessen benachbarte, freie Speicherbloecke
//miteinander verschmolzen werden.
void my_free(void* p)
{
	if(!b_initialized)
		return;
	//TODO
	//FREIGEBEN VON DEM ENTSPRECHENDEN SPEICHERBLOCK
	//
	memoryBlock *block = (memoryBlock *) ((void *)p - memoryBlockHeaderSize);
	block->state = not_allocated;
	//FREIE SPEICHERBLOECKE MITEINANDER VERSCHMELZEN
	mergeFreeBlocks();
}

//Diese Funktion verschmilzt benachbarte, nicht benutzte Speicherbloecke
void mergeFreeBlocks()
{
	//TODO 
	//IMPLEMENTIEREN
	memoryBlock *block = head;
	memoryBlock *freePos = NULL;
	int byteCount = 0;
	while (block != NULL)
	{
		//Beginn freier Bloecke
		if (block->state == not_allocated && freePos == NULL)
			freePos = block;
		//Weiterer freier Block
		if (block->state == not_allocated)
			byteCount += block->dataLength;
		//Ende freier Bloecke
		if (freePos != NULL)
		{
			freePos->state = not_allocated;
			freePos->data = (void *) freePos + memoryBlockHeaderSize;
			freePos->dataLength = byteCount;
			
			if (block->state == allocated)
			{
				freePos->nextBlock = block;
				byteCount = 0;
				freePos = NULL;
			}
			if (block->nextBlock == NULL)
			{
				byteCount = 0;
				freePos->nextBlock = NULL;
			}
		}
		block = block->nextBlock;
	}
}

//Diese Funktion gibt eine Uebersicht ueber die vorhandenen Speicherbl�cke aus
void status()
{
	if(!b_initialized)
	{
		initialize();
		b_initialized = 1;
	}
	memoryBlock *block = head;
	int count = 0;
	printf("Uebersicht des Speichers: %d / %d Speicher frei\n", get_free_space(), memorySize);
	printf("------------------------------------------------\n");
	printf("#  at\t\t allocated\t space\t data\t\t\tnext block\n");
	while(block != NULL)
	{

		printf("%d  %p\t %s \t\t %d\t [%p,%p]\t%p\n", ++count, block, boolStr[block->state], block->dataLength, (block->data), ((char*)block->data + block->dataLength-1), (block->nextBlock)); 
		block = block->nextBlock;
	}
	printf("\n\n");
}

