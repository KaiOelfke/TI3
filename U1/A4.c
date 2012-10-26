#include <stdio.h>
int main(int argc, const char * argv[])
{
    if (argc != 2) 
    {
        perror("Programm muss mit Datei gestartet werden!");
        return 1;
    }
    //Datei öffnen
    FILE *testFile = fopen(argv[1], "r");
    if (testFile != NULL)
    {
        double sum = 0;
        if (fscanf(testFile, "%lf", &sum) == 1)
        {
            double comparedSum = 0;
            double firstValue = 0;
            double secondValue = 0;
            while (fscanf(testFile, "%lf %*c %lf",&firstValue, &secondValue) == 2) comparedSum += firstValue * secondValue;
            if (comparedSum == sum) puts("Korrekt.");
            else puts("Falsch.");      
        }
        else 
        {
            perror("Fehler. Datei hat falsches Format.");
            return 1;
        }
        return 0;
    }
    else
    {
        perror("Fehler. Datei konnte nicht gelesen werden");
        return 1;
    }
}