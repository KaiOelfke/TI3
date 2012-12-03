/*

  1. Aufgabe

  Bei Linux wird aktuell ein Completely Fair Scheduler verwendent. Beim CFS 
  soll die Rechenzeit fair auf alle Prozesse aufgeteilt werden. Dafuer
  wird die Wartezeit jedes Prozesses gespeichert und der Prozess mit der
  hoechsten Wartezeit ist der naechste Prozess. Als Datenstruktur wird
  ein Rot-Schwarz-Baum verwendent mit Sortierung nach der Wartezeit. Je nach
  Sortierungsrichtung ist der Prozess mit der groessten Wartezeit ganz links
  bzw rechts. im Baum, weil Rot-Schwarz-Baeume zu den binaeren Suchbaeumen
  gehoeren. Im Worst-Case dauert es also Hoehe h Schritte bis zu diesem Prozess.
  Man kann beweisen, dass die Hoehe h eines RSB mit n Elementen (Prozessen hier)
  h <= log2(n + 1) ist. Wir haben also eine Komplexitaet von O(log(n)). Der Prozess
  wird geloescht, die Zeit aktualisiert und dann falls der Prozess nicht fertig
  ist wieder eingefuegt. Auch beim einfuegen muss man im Worst-Case bis zum
  unteren Ende des Baumes und deswegen auch h Schritte benoetigen. D.h. die
  gesamte Komplexitaet liegt bei O(log(n)).

  Mit sched_min_granularity_ns gibt man in Nanosekunden die minimale Granulari
  taet an und das hat zu Folge, dass ein Prozess mindestens so lange laeuft.

  Mit sched_latency_ns gibt man in Nanosekunden das Zeitintervall an in dem
  alle Prozesse einmal gescheduled wurden.

  Mit sched_wakeup_granularity_ns gibt man in Nanosekunden an, ab wann
  ein Prozess aufgeweckt werden kann um den aktuellen Prozess abzuloesen.
  Niedrige Werte bewirken, dass Prozesse schneller gewechselt werden.

  Mit diesen Parametern kann man u.a. den Scheduler auf Desktop oder Serversysteme
  optimieren.


  2. Aufgabe

  Bei einem Desktoprechner gibt es in der Regel einen Benutzer, der viele
  Programme (Internet, Musik, Textverarbeitung) "gleichzeitig" benutzen
  moechte und deswegen muss Multitasking moeglichst fluessig laufen. Dafuer
  ist es wichtig, dass Prozesse oft genug gewechselt werden, um sicherzugehen,
  dass jeder Prozess auch genug Rechenzeit bekommt und
  die Musik beispielsweise nicht kurz pausiert. 

  => Ausreichend viele Wechsel (z.B. Round Robin)

  Bei einem Server gibt es viele Anfragen und diese muessen moeglichst
  schnell bearbeitet werden. Hier sollten wenig Prozesswechsel stattfinden,
  da Wechsel kostenintensiv sind und die Bearbeitung verlangsamen. Es sollte
  also versucht werden eine Abfrage zunaechst zu beenden und 
  dann die naechste zu bearbeiten. Natuerlich muss garantiert werden,
  dass ein sehr langer Prozess nicht den Server fuer sehr lange Zeit blockiert.
  In der Regel haben, aber die Anfragen nur kurze Bearbeitungszeiten.

  => Minimal viele Wechsel (z.B. FCFS)

  3. Realtime Scheduler

  Realtimesysteme haben die Anforderung Operationen in einem gegebenen 
  Zeitrahmen stets abzuschliessen. Es muss also garantiert werden, dass
  definierte Deadlines eingehalten werden, weil sonst ein Systemfehler
  (hard realtime system) bzw. andere Nachteile (soft realtime system)
  auftreten. D.h. der scheduling Algorithmus muss garantieren,
  dass der Prozess mit der naehesten Deadline abgearbeitet wird 
  und bei Deadlineaenderungen muessen Prozesse gewechselt werden.
  Es ist meistens Aufgabe des Programmierers die Prozesse bzw. Deadlines
  so zu gestalten, dass eine Abarbeitung moeglich ist mit dem vorhandenen System.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



/*Implementierung eines Schedulers:
Implementieren Sie die Scheduling Algorithmen Round Robin,First Come First Serve und
Shortest Process Next.

Sie muessen hierbei nur die vorgefertigten Rahmenmethoden 

LINK roundRobin(LINK head,LINK current,int tStep);
LINK fcfs(LINK head,LINK current,int tStep);
LINK spn(LINK head,LINK current,int tStep);

implementieren.

TIPP:Machen Sie sich zunaechst mit dem Programmablauf vertraut und
implementieren Sie die Methoden anschliessend.
*/

#define PMAX 4 //Anzahl der Prozesse
#define TINT 500000 //Zeitintervall zwischen den Schritten des Algorithmus in Mikrosekunden
#define PDOTS 2//Anzahl der Punkte fuer die Anzeige eines Intervalls

struct pData //Daten der verketteten Liste
{
 int turns;//Wie oft war der Prozess schon dran
 int waited; //Wie lange musste er insgesamt warten
 int sTime;  //Wie viele Zeitschritte braucht er noch
 int nextStep;//wann moechte er als naechstes ausgefuehrt werden
 int pId;//ProzessID

 struct pData  *next;//Doppelt verkettete Liste
 struct pData  *prev; 
};

//Um Tipparbeit zu sparen
typedef struct pData PROCESS;
typedef PROCESS *LINK;

double g_stats[2]={0,0};//Globale Variable zum Aufzeichnen der Wartezeiten

//Methode zur Ausgabe der Wartezeiten nach dem Ausfuehren des Algorithmus
void printStats()
{
 printf("\n=============================================\n");

 printf("\nDie durchschnittliche Wartezeit der Prozesse \n fuer den Algorithmus betraegt:%f Zeitschritte\n",g_stats[0]/PMAX);
    
 printf("\nDie durchschnittliche Gesamtwartezeit \n fuer den Algorithmus betraegt:%f Zeitschritte\n",g_stats[1]/PMAX);
 g_stats[0]=0;
 g_stats[1]=0; 
}

//Methode zum updaten der stats fuer ein geloeschtes Element der Liste
void updateStats(LINK deleted)
{
 if(deleted->turns>0)              
  g_stats[0]+=deleted->waited/deleted->turns;//durschnittliche Zeit die der Prozess nicht dran war
  
  g_stats[1]+=deleted->waited;//Wartezeit          
}

void updateProcess(LINK current,int tStep);

LINK findShortestPr(LINK head);


void printSchedule(LINK current,int tStep);
void printScheduleHead(void);

LINK roundRobin(LINK head,LINK current,int tStep);
LINK fcfs(LINK head,LINK current,int tStep);
LINK spn(LINK head,LINK current,int tStep);
void schedule(LINK head,int processes[PMAX][2] ,LINK (*sAlgo)(LINK,LINK,int));

void addProcess(LINK head,int serviceTime,int tStep,int pId);
LINK deleteProcess(LINK current);

void printPr(LINK current);

void printList(LINK head);


int main(void)
{
LINK head=(LINK)malloc(sizeof(PROCESS));//das Kopfelement erzeugen
head->next=head;
head->prev=head;


int processes[PMAX][2]={{0,3},{2,7},{4,1},{6,5}};/*Prozesse die gescheduled werden
sollen.Das erste Element gibt die Ankunftszeit an.Das zweite die Ausfuerungsdauer des
Prozesses in Zeitschritten
*/


//Aufruf der Methoden
printf("\n Ablauf von First Come First Serve \n");
printScheduleHead();
schedule(head,processes,&fcfs);
printStats();

printf("\n Ablauf von RoundRobin \n");
printScheduleHead();
schedule(head,processes,&roundRobin);
printStats();




printf("\n Ablauf von Shortest Process Next\n");
printScheduleHead();
schedule(head,processes,&spn);
printStats();

return 0;
}

/*==========================================================================
Methoden fuer die Realisierung des Schedulers*/


//Einen Prozess an das Ende der Liste (also vor head) einfuegen
void addProcess(LINK head,int serviceTime,int tStep,int pId)
{
 //Neues Element erzeugen und initialisieren    
 LINK neu=(LINK)malloc(sizeof(PROCESS));
 
 neu->turns=0;
 neu->waited=0;
 neu->sTime=serviceTime;
 neu->nextStep=tStep;
 neu->pId=pId;	 

 //Zeiger so setzen,dass es das letzte Element ist
 head->prev->next=neu;
 neu->prev=head->prev;

 head->prev=neu; 
 neu->next=head;
}

//Loeschen des angegebenen Knotens
LINK deleteProcess(LINK current)
{
 //Update der Stats fuer diesen Knoten 
 updateStats(current);
 
 //naechsten Knoten zwischenspeichern     
 LINK tmp=current->next;    
 //Zeiger um den zu loeschenden Knoten herumsetzen
 current->next->prev=current->prev;
 current->prev->next=current->next;
 
 //Speicher freigeben
 free(current);
 current=NULL;
 
 return tmp;
}


/*Diese Methode ist fuer das eigentliche schedulen einr Methode zustaendig die die 
angegebene Signatur hat.Als Argumente erhaelt sie einen Zeiger auf die verkettete
Prozessliste,die auszufuehrenden Prozess und einen Zeiger auf die auszufuehrende Funktion
*/
void schedule(LINK head,int processes[PMAX][2] ,LINK (*sAlgo) (LINK,LINK,int))
{

 LINK current=head;
 int nextPrcs=0;
 int tStep=0;//aktueller Zeitschritt,der nach jeder Iteration erhoeht wird

 /*Fuehre das scheduling durch,solange noch Prozesse kommen werden oder
   noch Prozesse laufen
 */
  while(nextPrcs<PMAX||head->next!=head)
  {
                                        
   if(current==head)
    current=current->next;                                     
                                        
   /*Hole alle Prozesse ab,die im aktuellen Zeitschritt starten*/               
    while((nextPrcs<PMAX)&&(processes[nextPrcs][0]==tStep))
    {
     addProcess(head,processes[nextPrcs][1],processes[nextPrcs][0],nextPrcs);
     if(current==head)
    	current=head->prev;	
     nextPrcs++;
    }	
 
  
   if(current!=head)
    current=sAlgo(head,current,tStep);
   tStep++;		
   usleep(TINT);
  }	
}

/*Diese Methode fuehrt den Prozess current aus indem die serviceTime
dekrementiert wird und die restlichen Elemente aktualisiert werden
*/
void updateProcess(LINK current,int tStep)
{
 printSchedule(current,tStep);  

 current->turns++;
 current->waited+=tStep-current->nextStep;
 current->nextStep=tStep+1;
 current->sTime--;    
}

/*Implementierung des Round Robin Algorithmus
head:Kopfelement der Liste der Prozesse
current:aktuelles Element
tStep:aktueller Zeitschritt
*/

LINK roundRobin(LINK head,LINK current,int tStep)
{       
 updateProcess(current,tStep); 
/*TODO:implement method*/
 LINK temp;
 if (current->sTime==0)
    temp = deleteProcess(current);
 current=current->next;
 return current;
}


/*Implementierung des FCFS

head:Kopfelement der Liste der Prozesse
current:aktuelles Element
tStep:aktueller Zeitschritt
*/
LINK fcfs(LINK head,LINK current,int tStep)
{
  updateProcess(current,tStep); 

  /*TODO:implement method*/

  LINK  temp;
  if (current->sTime==0)
    temp = deleteProcess(current);
  current=head->next;
  return current;
}


/*Implementierung des spn
head:Kopfelement der Liste der Prozesse
current:aktuelles Element
tStep:aktueller Zeitschritt
*/
LINK spn(LINK head,LINK current,int tStep)
{
  updateProcess(current,tStep); 

  /*TODO:implement method*/
  LINK temp;
 
  if (current->sTime==0)
    temp = deleteProcess(current);

  current = findShortestPr(head);
  return current;
}

/*Hilfsfunktion fuer den spn Algorithmus*/

LINK findShortestPr(LINK head)
{
     LINK curr=head->next;
     LINK minCurr=curr;
     int sMin=curr->sTime;
     while(curr!=head)
     {
      if(sMin>curr->sTime)
      { 
         sMin=curr->sTime;                 
         minCurr=curr;
      } 
      curr=curr->next;               
     }
     return minCurr;
}

/*Anzeigemethoden fuer die einzelnen Prozesse*/
void printSchedule(LINK current,int tStep)
{
 int i;
 char *out=(char*)malloc(7*sizeof(char)*current->pId+10) ;
 strcpy(out,"\t");
 for(i=0;i<current->pId;i++)
 {
  strcat(out," \t    ");
 }
 strcat(out,".\n ");
 
 printf("%d%s",tStep,out);
 for(i=0;i<PDOTS;i++)
 {
  printf(" %s",out);
 } 
 free(out);
}


void printScheduleHead(void)
{
 int i;
 char * out=(char*)malloc(10+sizeof(char)*10*PMAX);

 strcpy(out," tstep");

 for(i=0;i<PMAX;i++)
 {
  sprintf(out,"%s\t Pid %d ",out,i);
 }

 printf("\n=============================================\n");
 printf("%s \n ",out);
 free(out);
}


/*Ausgabemethoden fuer die Prozesse und die gesamte Liste (DEBUG-Methoden)*/
void printPr(LINK current)
{
 printf("\n pId:%d turns:%d waited:%d sTime:%d nextStep:%d \n ",current->pId,current->turns,current->waited,current->sTime,current->nextStep);
}

void printList(LINK head)
{
     printf("\n\n Current QUEUE of PROCESSES ---------------------------------------\n\n");
     LINK curr=head->next;
     while(curr!=head)
     {           
      printPr(curr);  
      curr=curr->next;                     
     }     
}


