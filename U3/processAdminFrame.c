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

struct pData //Daten der doppelt verketteten Liste
{
 int pId;             //ProzessID
 int aTime;           //Ankunftszeit
 int sTime;           //Ausfuehrungszeit
 struct pData *left;  //die linke zusammengesetzte Daten
 struct pData *right; //die richtige zusammengesetzte Daten
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
 LINK next;  // Fuer While-Schleife

 // head initialisieren 

 head = (LINK)malloc(sizeof(PROCESS)); 
 head->pId = 0;
 head->aTime = 0;
 head->sTime = 0;
 head->left = head;
 head->right = head;
 
 readProcesses(head);
 while(head->right!=head)
 {
   printList(head);                     
   next=findNextProcess(head);
   if (next == head) return -1;
   deleteProcess(next);                   
 }

 return 0;
}

/*==========================================================================
Implementierung der Methoden*/


//Liest die Prozesse ein und erstellt die Liste
void readProcesses(LINK head)
{ 
FILE *pFile = fopen("prcs.dat", "r");
 if (pFile != NULL)
    {
        int prozessId,arrivalTime,executionTime;
        while (fscanf(pFile, "%d %d %d",&prozessId, &arrivalTime,&executionTime) == 3)
        {
          LINK next = (LINK)malloc(sizeof(PROCESS));
          next->aTime = arrivalTime;
          next->sTime = executionTime;
          next->pId = prozessId;
          addProcess(head,next);
        }
        fclose(pFile);
    }
    else
    {
        perror("Fehler. Datei konnte nicht gelesen werden");
    }

}


//Einen Prozess an das Ende der Liste (also vor head) einfuegen
void addProcess(LINK head,LINK neu)
{
  neu->right       = head;
  neu->left        = head->left;
  head->left       = neu;
  neu->left->right = neu;
}

//Loeschen des angegebenen Knotens
void deleteProcess(LINK current)
{
  current->left->right = current->right;
  current->right->left = current->left;
  free(current);
}


/*gibt den Prozess mit der kleinsten Id aus der Liste zurueck*/

LINK findNextProcess(LINK head)
{
   // deklariere einen Zeiger, der auf head->right zeigt.

   LINK zeiger;
   zeiger = head->right;
   int kleinste = zeiger->pId;
   // die kleinste Id finden.
   while(zeiger!=head){
      if (zeiger->pId < kleinste) kleinste = zeiger->pId;
      zeiger = zeiger->right;
   }
   // Id finden und dann den Zeiger zurueckgeben.
   zeiger = head->right;
   while(zeiger!=head){
      if (zeiger->pId ==kleinste) return zeiger;
      zeiger = zeiger->right;
   }
   return zeiger;
}


/*Ausgabemethoden fuer die Prozesse und die gesamte Liste*/
void printPr(LINK current)
{
 printf("\n pId:%d arrival time:%d service time:%d\n ",current->pId,current->aTime,current->sTime);
}

void printList(LINK head)
{
     printf("\n\n Current QUEUE of PROCESSES ---------------------------------------\n\n");
     LINK curr=head->right;
     while(curr!=head)
     {           
      printPr(curr);  
      curr=curr->right;                     
     }     
}


