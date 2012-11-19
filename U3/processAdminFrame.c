/*
  1. Aufgabe:
Erklären Sie den Sinn und die Funktion der einzelnen Felder im Process Control Block.

  In dem PCB werden wichtige Informationen für das BS pro Prozess gespeichert,
  die fuer die Prozessverwaltung benötigt werden.

  Felder:
  - Process identifiers: IDs, Elternprozesse, Benutzer
  - Für Identfikation und Einordnung / Zurodnung

  - CPU Zustand: Benutzerregister, Kontroll-, Statusregister
  - Für Zustandssicherung bei Nebenlaeufigkeit / Interrupts, 

  - Kontrollinformationen:
  - Scheduling, Acccounting, Memory management, I/O management
  - Für Berechnung, wann der Prozess ausgefuehrt werden soll
  - Für Speicherung, des Speicher und CPU Zeit Verbrauchs (dadurch kann es z.B.
  im Task Manager oder Activity Monitor angezeigt werden)
  - Um Zugriffsrechte auf den Speicher und Speicherorte (Adressen) zu verwalten
  - Um Ein und Ausgabe zu verwalten also z.B. das ein Prozess gerade eine Datei x
  schreibt

  2. Aufgabe:
Erklären Sie den fork() Befehl und seine Funktionsweise.

  Mit fork() kann man in einem Prozess einen neuen Kindprozess erstellen
  mit einer neuen PID und die PID vom aufrufenden Prozess wird die Parent PID.
  Code und Daten werden dabei kopiert, wobei Referenzen auf Dateien nur kopiert
  werden und damit immer noch auf dieselben Objekte zeigen. Dadurch werden
  Dateien z.B. geteilt.

  fork liefert eine 0 zurueck, wenn man im Kindprozess ist und einen Wert > 0,
  wenn man im Elternprozess ist. Dadurch kann man also parallel unterschiedliche
  Pfade im Programm ausfuehren. Also z.B. auf unterschiedlichen Kernen oder
  nacheinander mit Scheduling. Dies ist praktisch, wenn man ein "Multitasking"
  Verhalten realisieren will. So kann man z.B. in einem Music Player fork() 
  benutzen und wenn es der Kindprozess ist wird eine Routine ausgefuehrt, um
  Musik wiederzugeben und beim Elternprozess werden die normalen Funktionen
  bearbeitet, wie zum Beispiel GUI Events.

  3. Aufgabe:
  Grenzen Sie Task, Prozess und Thread voneinander ab.
  
  Ein Prozess ist ein durch die Befehle definierte algorithmische Verarbeitung,
  der im Speicher geladen ist und Ressourcen bekommt wie CPU Zeit und Speicher.

  In einem Prozess kann man mehere Ausfuehrungspfade realisieren mit Hilfe von 
  Threads. Dabei werden Daten und Befehle geteilt, aber unterschiedliche Threads
  erledigen oft unterschiedliche Teilaufgaben des Prozesses. Dadurch soll eine 
  Performancesteigerung durch Parallelisierung erreicht werden. Z.B. kann ein
  Music Player mehrere Threads erstellen um Musik abzuspielen und die grafische
  Oberflaeche getrennt zu verwalten. Wenn dann z.B. eine Grafikfunktion nicht 
  schnell genug beendet werden kann und ein Timerinterrupt auftritt wird dadurch
  die Musik nicht gestoppt. In diesem Fall wuerde die Anwendung also der Music 
  Player seine Threads verwalten. Es sind also User Threads. Es gibt auch
  verschiedene Ausfuehrungspfade aus Sicht des BS. Diese nennt man Kernel Threads.
  Ein Prozess kann also mehrere Threads haben, die Daten und Adressbereiche teilen, 
  aber unterschiedliche Dinge erledigen oder gemeinsam schneller eine Sache erledigen. 
  Prozesse dagegen koennen komplett andere Algorithmen haben und Speicherbereiche bzw.
  Adressbereiche. 
  Ein Task ist definiert als eine Aufgabe. Sowohl ein Programm als auch ein Thread
  kann eine gewisse Aufgabe erledigen. Es gibt hier keine eindeutige Festlegung. 
  Tasks werden manchmal synonym zu Threads oder Prozessen verwendet.

*/


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


