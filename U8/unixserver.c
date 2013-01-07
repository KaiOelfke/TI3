#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>
#include <stdio.h>
#include <strings.h>

int main(int argc, char *argv[])
{
	int socketFileDescriptor;
	int newSocketFileDescriptor;
	int serverAddressLength;
	int n;
	struct sockaddr_un serverAddress;
	struct sockaddr_un clientAddress;
	socklen_t clientLength;
	char buffer[80];
	if (argc != 2)
	{
		perror("Keine Adresse angegeben.")
		exit(0);
	}
	if ((socketFileDescriptor = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
	{
		perror("Socket konnte nicht erstellt werden.");
		exit(0);
	} 
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sun_family = AF_UNIX;
	strcpy(serverAddress.sun_path, argv[1]);
	serverAddressLength = strlen(serverAddress.sun_path) + sizeof(serverAddress.sun_family);
	if(bind(socketFileDescriptor,(struct sockaddr *)&serverAddress,serverAddressLength ) <0)
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
	n = read(newSocketFileDescriptor,buffer,80);
	printf("Eine Verbindung wurde hergestellt.\n%s",buffer);
	write(newSocketFileDescriptor,"Hallo Client\n",13);
	close(newSocketFileDescriptor);
	close(socketFileDescriptor);
	return 0;
}
