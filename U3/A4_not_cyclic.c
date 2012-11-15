#include <stdio.h>
#include <stdlib.h>


/*Implementierung einer Prozessverwaltung:

Implementieren Sie die durch das Framework vorgegebene Prozessverwaltung,
die eine Menge von Prozessen aus der Datei prcs.dat auslesen,und diese
in eine doppelt verkettete Liste einfuegen soll.

Nun soll Prozess um Prozess der Reihenfolge ihrer ids entsprechend 
aus der Liste entfernt werden,wobei die uebrig gebliebenen Prozesse in jeder
Iteration ausgegeben werden.

Die Prozesse sind in der Datei prcs.dat Zeile fuer Zeile in der Form:
				ProzessId,Ankunftszeit,Ausfuehrungszeit
gespeichert.    

*/

#define PROCESS_LIST_FILENAME "prcs.dat"

struct pData //Daten der doppelt verketteten Liste
{
	int pId;//ProzessID
	int aTime;//Ankunftszeit
	int sTime;//Ausfuehrungszeit
	struct pData *prev;
	struct pData *next;
		/*Struktur vervollstaendigen */
};

//Um Tipparbeit zu sparen
typedef struct pData PROCESS;
typedef PROCESS *LINK;

LINK findNextProcess(LINK head);

void addProcess(LINK head,LINK neu);
void deleteProcess(LINK current);

void printPr(LINK current);//Methode zur Ausgabe eines Prozesses

void printList(LINK head);//Alle Prozesse der Liste ausgeben

void readProcesses(LINK head);//Einlesen der Prozesse

int main(void)
{
	LINK head;

	/*TODO:head initialisieren*/

	head = malloc(sizeof(PROCESS));
	if (head == NULL)
	{
		printf("Nicht ausreichend Speicher vorhanden.\n");
		return EXIT_FAILURE;
	}
	head->next = NULL;
	head->prev = NULL;
	head->pId = -1;
	head->aTime = -1;
	head->sTime = -1;
	readProcesses(head);

	LINK current;
	while(head->next != NULL)
	{
			printList(head);                     
			current = findNextProcess(head);
			deleteProcess(current);                   
	}
	free(head);
	return 0;
}

/*==========================================================================
Implementierung der Methoden*/


//Liest die Prozesse ein und erstellt die Liste
void readProcesses(LINK head)
{
	int prozessId,arrivalTime,executionTime;
	FILE *processFile = fopen(PROCESS_LIST_FILENAME, "r");
	if (processFile == NULL)
	{
		perror("Prozessliste konnte nicht geoeffnet werden:");
		return;
	}
    while (fscanf(processFile, "%d %d %d",&prozessId, &arrivalTime,&executionTime) == 3)
    {
		LINK next = (LINK) malloc(sizeof(PROCESS));
		next->aTime = arrivalTime;
		next->sTime = executionTime;
		next->pId = prozessId;
		addProcess(head,next);
    }
    fclose(processFile);
}


//Einen Prozess an das Ende der Liste (also vor head) einfuegen
void addProcess(LINK head, LINK neu)
{
	LINK current = head;
	while (current->next != NULL && current->next->pId < neu->pId)
		current = current->next;
	if (current->next == NULL)
	{
		current->next = neu;
		neu->next = NULL;
		neu->prev = current;
	}
	else
	{
		neu->next = current->next;
		neu->prev = current;
		current->next = neu;
	}
}

//Loeschen des angegebenen Knotens
void deleteProcess(LINK current)
{
	if (current->prev == NULL && current->next != NULL)
		current->next->prev = NULL;
	else if (current->next == NULL && current->prev != NULL)
		current->prev->next = NULL;
	else if (current->prev != NULL && current->next != NULL)
	{
		current->prev->next = current->next;
		current->next->prev = current->prev;
	}
 	free(current);
}


/*gibt den Prozess mit der kleinsten Id aus der Liste zurueck*/

LINK findNextProcess(LINK head)
{
	return head->next;
}


/*Ausgabemethoden fuer die Prozesse und die gesamte Liste*/
void printPr(LINK current)
{
	printf("\n pId:%d arrival time:%d service time:%d\n ",current->pId,current->aTime,current->sTime);
}

void printList(LINK head)
{
					printf("\n\n Current QUEUE of PROCESSES ---------------------------------------\n\n");
					LINK curr = head->next;
					while(curr != NULL)
					{           
						printPr(curr);  
						curr=curr->next;                     
					}     
}


