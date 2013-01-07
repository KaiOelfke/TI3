#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <strings.h>
int main(int argc, char *argv[])
{
	int socketFileDescriptor;
	int serverAddressLength;
	int n;
	struct sockaddr_un serverAddress;
	char buffer[82];

	//bzero ueberschreibt struct mit Nullen
	bzero((char *)&serverAddress,sizeof(serverAddress));
	//Wir wollen Unix Domain Sockets
	serverAddress.sun_family = AF_UNIX;
	//Pfad fuer den Socket aus Aufruf sun = socket unix != Sonne
	if (argc != 2)
	{
		perror("Keine Adresse angegeben.")
		exit(0);
	}
	strcpy(serverAddress.sun_path, argv[1]);
	//Ermittle Laenge der Serveradresse fuer connect()
	serverAddressLength = strlen(serverAddress.sun_path) + sizeof(serverAddress.sun_family);
	/*
	SOCK_STREAM
	stellt einen sequenziellen, verlässlichen, zwei-wege, verbindungsbasierten
	Byte-Stream zur Verfügung. 
	Ein lqout-of-bandrq Übertragungsmechanismus kann unterstützt werden.
	http://www.linuxhowtos.org/data/6/U_client.c
	*/
	if ((socketFileDescriptor = socket(AF_UNIX, SOCK_STREAM,0)) < 0)
	{
		perror("Socket konnte nicht erstellt werden.");
		exit(0);
	}
	if (connect(socketFileDescriptor, (struct sockaddr *) &serverAddress, serverAddressLength) < 0)
	{
		perror("Verbindung konnte nicht hergestellt werden.");
		exit(0);
	}
	printf("Eingabe max. 70 Zeichen:\n");
	bzero(buffer,82);
	fgets(buffer,80,stdin);
	write(socketFileDescriptor,buffer,strlen(buffer));
	n = read(socketFileDescriptor,buffer,80);
	printf("Antwort:\n%s",buffer);
	close(socketFileDescriptor);
	return 0;
}
