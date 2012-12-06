/*1. Aufgabe: Asynchrone IO

Die Eingabe und Ausgabeprozesse sind bei Computern oft sehr viel langsamer als die CPU. Der Grund dafuer ist, dass
bei Eingabe und Ausgabe oft mechanische Elemente bewegt werden muessen wie bei einer DVD oder Festplatte und in der Zeit
kann ein moderner Prozesser mit einer Taktfrequenz im GHz Bereich wesentlich mehrere Befehle ausfuehren. Bei dem sychronen
Verfahren wartet man einfach auf die Abarbeitung der Ein bzw. Ausgabe und pausiert solange die CPU. Vorteilhaft ist hier, dass
es zu keinen Fehlern kommt, weil die Sequenz des Programms eingehalten wird und es ist einfacher zu implementieren. Der grosse
Nachteil hierbei ist, aber dass man eine lange Wartezeit hat und in der Zeit auch sehr gut andere Sachen berechnen koennte. 
Ein Programm wird also durch synchrone Ein und Ausgabe sehr langsam. Die Alternative ist asynchrone IO. Hierbei startet man
den IO Prozess und solange kein Ergebnis da ist, werden andere Berechnungen durchgefuehrt, die nicht das Ergebnis von dem IO 
Prozess benoetigen. Wenn man zum Beispiel ein Spiel hat bei dem die Spielstaende lokal und auf einem Server gespeichert werden
koennte man den Spieler ein neues Spiel beginnen lassen, was dann zunaechst erstmal lokal gesichert wird und der Spieler muss
nicht warten bis die Sychronisierung der Spielstaende abgeschlossen ist. Synchrone nicht blockierende APIS werden auch synchron
ausgefuehrt, aber wenn ein Ergebnis nicht sofort geliefert werden kann (es wird schon auf die Datei zugegriffen...), beendet die
Methode sofort und meldet, dass die Ressource gerade nicht verfuegbar ist.

Beispiel: Telefon Hotline

Synchron: Kunde ruft an, Support sagt, dass Kunde in der Leitung bleiben soll, Kunde wartet in der Leitung 10 Min und kann nichts machen
Asynchron: Kunde ruft an, Support sagt: Ich kuemmere mich darum und rufe zurueck, sobald ich es habe. Kunde kann solange etwas anderes machen.
Synchron Non Blocking: Kunde ruft an, Support sagt: Hier ist das Ergebnis oder momentan ist xy nicht verfuegbar. Versuchen Sie es spaeter noch einmal.

2. Aufgabe

Bei dem FAT16 Dateisystem ist der Ort und die Groesse der Root Directory festgelegt. Dadurch ergibt sich die Beschraenkung von der Dateianzahl.
Bei FAT32 wird dagegen die Root Directory nicht mehr festgelegt und deswegen koennen mehr Dateien gespeichert werden. Unterverzeichnisse liegen
in der File and Directory Data Region und sind nur durch den Plattenspeicher limitiert. Es koennen 2^16 Cluster addressiert werden mit einer 16 Bit
Adresse. Daraus resultieren 65.536 - 12 (interne Verwaltung) 65524 Cluster und damit koennen dann maximal so viele Dateien angelegt werden. Ein Cluster
ist hierbei maximal 32 MB = 2 * 1024 * 1024 KB Groesse / 2^16 Cluster.°
*/

#include <stdio.h>
#define BUFFER_SIZE (64 * 1024)

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		perror("Programm muss mit mycat file_name aufgerufen werden.");
		return 1;
	}
	FILE* file = fopen(argv[1],"r");
	if (file)
	{
		char buffer[BUFFER_SIZE];
		while (fgets(buffer, BUFFER_SIZE, file) != NULL)
			fputs(buffer, stdout);
		fclose(file);
		return 0;
	}
	else
	{
		perror("Konnte Datei nicht lesen.");
		fclose(file);
		return 1;
	}
}
