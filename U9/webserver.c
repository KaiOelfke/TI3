#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>


int main(int argc, char *argv[])
{
	int socketFileDescriptor;
	int newSocketFileDescriptor;
	int portNumber = 80;
	int n;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	socklen_t clientLength;
	char buffer[2048];

	if ((socketFileDescriptor = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("Socket konnte nicht erstellt werden:");
		exit(0);
	} 
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(portNumber);
	if(bind(socketFileDescriptor,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) <0)
	{
		perror("Konnte Socket nicht binden:");
		exit(0);
	} 
	listen(socketFileDescriptor,10);
	fd_set readset;
	FD_ZERO(&readset);	
	FD_SET(socketFileDescriptor,&readset);
	int max = socketFileDescriptor;
	for (;;)
	{
		struct fd_set readset_copy;
		FD_ZERO(&readset_copy);
		FD_COPY(&readset, &readset_copy);
		struct timeval timeout;
		timeout.tv_sec = 60;
		if (select(max + 1, &readset_copy, NULL, NULL, &timeout) < 0)
		{
			perror("Fehler bei select():");
			exit(0);
		}
		FD_ISSET(socketFileDescriptor, &readset_copy);
		clientLength = sizeof(clientAddress);
		newSocketFileDescriptor = accept(socketFileDescriptor,(struct sockaddr *)&clientAddress,&clientLength);
		if (newSocketFileDescriptor < 0) 
		{
			perror("Konnte Verbindung nicht akzeptieren:");
			exit(0);
		}
		FD_SET(newSocketFileDescriptor, &readset);
		if (newSocketFileDescriptor > max)
			max = newSocketFileDescriptor;
		for (newSocketFileDescriptor = socketFileDescriptor + 1; newSocketFileDescriptor <= max; ++newSocketFileDescriptor)
			if (FD_ISSET(newSocketFileDescriptor, &readset_copy))
			{
				n = read(newSocketFileDescriptor, buffer, sizeof(buffer));
				if (n < 0)
					FD_CLR(newSocketFileDescriptor, &readset);
				else
				{
					write(newSocketFileDescriptor,"Hallo Client\n",13);
				}
			}
	}






















/*

	
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
		fclose(logFile);*/
	close(newSocketFileDescriptor);
	close(socketFileDescriptor);
	return 0;
}
