#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
int main(int argc, char *argv[])
{
	int socketFileDescriptor;
	int newSocketFileDescriptor;
	int portNumber;
	int n;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	socklen_t clientLength;
	char buffer[300];
	if (argc != 3)
	{
		perror("Portangabe oder Logdateipfad fehlt.");
		exit(0);
	}
	FILE * logFile = fopen(argv[2], "a");
	if ((socketFileDescriptor = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("Socket konnte nicht erstellt werden.");
		exit(0);
	} 
	bzero((char *) &serverAddress, sizeof(serverAddress));
	portNumber = atoi(argv[1]);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(portNumber);
	if(bind(socketFileDescriptor,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) <0)
	{
		perror("Konnte Socket nicht binden");
		exit(0);
	} 
	listen(socketFileDescriptor,5);
	clientLength = sizeof(clientAddress);
	newSocketFileDescriptor = accept(socketFileDescriptor,(struct sockaddr *)&clientAddress,&clientLength);
	if (newSocketFileDescriptor < 0) 
	{
		perror("Konnte Verbindung nicht akzeptieren");
		exit(0);
	}
	char ip_adress[50];
	inet_ntop(AF_INET, &clientAddress.sin_addr.s_addr, ip_adress, clientLength);
	time_t rawtime;
	struct tm *timeinfo;
	char timestring[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime (timestring,80,"%d.%m.%Y %H:%M:%S",timeinfo);
	fprintf(logFile,"%s %s:%i\n",timestring,ip_adress,ntohs(clientAddress.sin_port));
	n = read(newSocketFileDescriptor,buffer,200);
	printf("Eine Verbindung wurde hergestellt.\n%s",buffer);
	write(newSocketFileDescriptor,"Hallo Client\n",13);
	close(newSocketFileDescriptor);
	close(socketFileDescriptor);
	fclose(logFile);
	return 0;
}
