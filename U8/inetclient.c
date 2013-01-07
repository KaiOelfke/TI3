#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <strings.h>
#include <netdb.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
	int socketFileDescriptor;
	int portNumber;
	int n;
	struct sockaddr_in serverAddress;
	struct hostent *server;
	char buffer[300];

	if (argc != 2)
	{
		perror("Portangabe fehlt.");
		exit(0);
	}
	portNumber = atoi(argv[1]);
	//Wir wollen Internet Sockets
	/*
	SOCK_STREAM
	stellt einen sequenziellen, verlässlichen, zwei-wege, verbindungsbasierten
	Byte-Stream zur Verfügung. 
	Ein lqout-of-bandrq Übertragungsmechanismus kann unterstützt werden.
	*/
	if ((socketFileDescriptor = socket(AF_INET, SOCK_STREAM,0)) < 0)
	{
		perror("Socket konnte nicht erstellt werden.");
		exit(0);
	}
	server = gethostbyname("127.0.0.1");
	if (server == NULL) {
        perror("Host existiert nicht.");
        exit(0);
    }
    bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *) &serverAddress.sin_addr.s_addr,server->h_length);
	//Konvertiert portNumber von host in network byte order Little/BigEndian
	serverAddress.sin_port = htons(portNumber);	
	if (connect(socketFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("Verbindung konnte nicht hergestellt werden.");
		exit(0);
	}
	printf("Eingabe max. 200 Zeichen:\n");
	bzero(buffer,300);
	fgets(buffer,200,stdin);
	write(socketFileDescriptor,buffer,strlen(buffer));
	n = read(socketFileDescriptor,buffer,200);
	printf("Antwort:\n%s",buffer);
	close(socketFileDescriptor);
	return 0;
}
