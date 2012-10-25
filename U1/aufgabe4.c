#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void einlesen( char name[] , char text[])
{
   int n = 0;
   char c;
   FILE * file;
   file = fopen (name , "r");
   if (file == NULL) perror ("Error opening file");
   else {
   	// lese byte weise . 
   	// dann speichere jedes elment "char" in dem array.
 	while (c != EOF) {
      c = getc (file);
      text[n]=c;
      n++;
    }
   }
   fclose (file);
}

short rechnen(char zahlen[]){
	int zahl = 0;
	int ersteoperand = 0;
	int zweiteoperand = 0;
	int summe = 0;
	int counter = 0;
	int n = 0;
	int x = 0;
	int lengthofstring = strlen(zahlen);	// länge von dem Text .
	char tempzahl[10];						// 10  weil ersteoperand und zweiteoperand sind int .
	char temperst[10];
	char tempzwei[10];
	for (; n<lengthofstring;n++){
		if (zahlen[n] == '\n' || zahlen[n]==EOF){
			if (zahl == 0){
				zahl = atoi(tempzahl);
			}
			else {
				for (int i = 0; i <=counter;i++){
					if (tempzahl[i]=='*'){
						 x = 0;
						for (; x<i; x++){
							temperst[x] = tempzahl[x];
						}
						ersteoperand = atoi(temperst);
						x = 0;
						i++;
						for (; i<=counter;i++){
							tempzwei[x] = tempzahl[i];
							x++;
						}
						zweiteoperand = atoi(tempzwei);
						summe = summe + (zweiteoperand * ersteoperand);
						break;
					}
				}
			}
			int i = 0;
			x = 0;
			for (; i <=counter;i++){
				tempzahl[i] = 0;
				temperst[i] = 0;
				tempzwei[i] = 0;

			}
			ersteoperand = 0;
			zweiteoperand = 0;
			counter = 0;
		}
		else {
			tempzahl[counter]= zahlen[n];
			counter++ ;
		}
	}
	//printf("ersteoperand %i zweiteoperand %i summe %i zahl %i \n",ersteoperand,zweiteoperand,summe,zahl );
	// Überprüfe ob die zahl , die in der erste zeile ist . gleich was das program gerechnet habe.
	if (summe == zahl){
		return 1;
	}
	return 0;


}	

int main(){

	// char *String = (char *) malloc(1000);
	// statt 1000 ein funkion,die zählt wie char haben wir in dem text .. 
	// also  mit dem können wir ein array ohne bestimmte größe erzeugen .
	// danach mit funktion free löschen wir was wir reserviert haben . ;) 
   char zahlenzeilen [256];
   einlesen("text.txt",zahlenzeilen);
   //printf("%s\n", zahlenzeilen);
   short value = rechnen(zahlenzeilen);
   if (value == 1){
   	printf(" Richtig ;)\n");
   }


   return 0;
}