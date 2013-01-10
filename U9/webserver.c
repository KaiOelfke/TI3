/*
   1. Aufgabe: Begriffe
      Beschreiben Sie jeden der folgenden Begriffe durch maximal zwei Sätze:
      Bit, Signal, Frequenzmodulation, Framing, Fehlererkennung

      Bit:
      Ein Bit ist eine theoretische Einheit mit zwei moeglichen Zustaenden (1 und 0),
      welche durch verschiedene Systeme mit zwei unterscheidbaren Zustaenden implementiert werden kann.

	  Signal:
	  Ein Signal ist ein Zeichen bzw. eine Zeichenkette übertragen durch ein Übertragungsmedium wie ein Kabel und symbolisiert meistens
	  eine gewisse Nachricht (z.B. ein Bit oder Byte) je nach Einsatzort.

	  Frequenzmodulation:
	  Bei der Frequenzmodulation wird die Frequenz des Signals verändert. Mit Hilfe der Frequenzmodulation kann man Daten übertragen,
	  indem man zum Beispiel festlegt, dass eine niedrige Frequenz 0 und eine hohe 1 bedeutet.

	  Framing: 
	  Das Signal wird unterteilt in Frames (= Pakete). Wobei das Problem ist das Ende und
	  den Anfang von jedem Frame zu finden.

	  Fehlererkennung:
	  Fehlererkennung ist ein Verfahren wie CRC um die ungültige Pakete whärend der Übertragung zu erkennen.
	


	2. Aufgabe: Datenrate
	  Ein Kanal habe eine Bandbreite von 2 MHz. Wie viele Bit/s können gesendet werden,
	  wenn digitale Signale mit 6 Levels verwendet werden (nehmen sie einen rauschlosen Kanal an)?

 	  die Formel von Nyquist formula:
	  Daterate = 2H log2 V 
	  V = 4 , 2^(abgerundet(log2 6))
	  H = 2 MHz
	  Die Daterate = (2 * (2000) * 1000 * log2 (4)) / 1000 / 1000 
	  Die Daterate = 8 Mbitps

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <strings.h>
#include <errno.h>

#define MaxConnections (10)
#define HTTP404NotFound "HTTP/1.1 404 Not Found\r\n"
#define HTTP200OK "HTTP/1.1 200 OK\r\n"
#define HTTPContentType "Content-Type: "
#define HTTPConnection "Connection: close\r\n"
#define HTTPContentLength "Content-Length: "
#define HTTPMIMETextHTML "text/html\r\n"
#define HTTPMIMEImage "image/png\r\n"

void error(char *message)
{
	perror(message);
	exit(0);
}

int initialize(int port)
{
	int serverSocket;
	struct sockaddr_in serverAddress;
	if ((serverSocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
		error("Socket konnte nicht erstellt werden:");
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	if(bind(serverSocket,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) < 0)
		error("Konnte Socket nicht binden:");
	if (listen(serverSocket,MaxConnections) < 0)
		error("listen() fehlgeschlagen");
	return serverSocket;

}

void parseRequest(char *request, char *page)
{
	if (sscanf(request, "%*s %s", page) != 1)
		page[0] = '\0';
	else
	{
		if (strcmp(page, "/") == 0)
			sprintf(page, "%s", "www/index.html");
 		else
 		{
 			char temp[strlen(page)];
 			strcpy(temp, page);
 			sprintf(page, "www/");
 			page += 4;
 			sprintf(page, "%s", temp);
 		}
	}
}

long fileLength(FILE *file)
{
	fseek(file, 0, SEEK_END);
	int ret = ftell(file);
	rewind(file);
	return ret;
}

void writeToSocket(char *buffer, int writeSocket, int bytesToWrite)
{
	char *writePostion = buffer;
	while (bytesToWrite > 0)
	{
		int bytesWritten = write(writeSocket, buffer, bytesToWrite);
		if (bytesWritten == -1)
		{
			if (errno == EINTR)
				continue;
			else
				error("Fehler beim Schreiben");
		}
		bytesToWrite -= bytesWritten;
		writePostion += bytesWritten;
	}
}

void writeHTTPHeader(char *page, FILE *pageFile, int writeSocket)
{
	char length[32];
	if (pageFile == NULL)
	{
		writeToSocket(HTTP404NotFound, writeSocket, strlen(HTTP404NotFound));
		writeToSocket(HTTPContentType, writeSocket, strlen(HTTPContentType));
		writeToSocket(HTTPMIMETextHTML, writeSocket, strlen(HTTPMIMETextHTML));
		writeToSocket(HTTPConnection, writeSocket, strlen(HTTPConnection));
		writeToSocket(HTTPContentLength, writeSocket, strlen(HTTPContentLength));
		pageFile = fopen("www/404.html", "r");
		sprintf(length, "%ld\r\n", fileLength(pageFile));
		fclose(pageFile);
		writeToSocket(length, writeSocket, strlen(length));
	}
	else
	{
		writeToSocket(HTTP200OK, writeSocket, strlen(HTTP200OK));
		writeToSocket(HTTPContentType, writeSocket, strlen(HTTPContentType));
		char *pos = strchr(page,'.');
		if (strcmp(pos,".png") == 0)
			writeToSocket(HTTPMIMEImage, writeSocket, strlen(HTTPMIMEImage));
		else
			writeToSocket(HTTPMIMETextHTML, writeSocket, strlen(HTTPMIMETextHTML));
		writeToSocket(HTTPConnection, writeSocket, strlen(HTTPConnection));
		writeToSocket(HTTPContentLength, writeSocket, strlen(HTTPContentLength));
		sprintf(length, "%ld\r\n\r\n", fileLength(pageFile));
		writeToSocket(length, writeSocket, strlen(length));
	}
}

void writeResponse(int writeSocket, char *page)
{
	FILE *pageFile = fopen(page, "r");
	if (pageFile == NULL)
	{
		pageFile = fopen("www/404.html", "r");
		writeHTTPHeader(page, NULL, writeSocket);
	}
	else
		writeHTTPHeader(page, pageFile, writeSocket);
	char buffer[2048];
	int bytesRead = 0;
	while (!feof(pageFile)) 
	{
		bytesRead = fread(buffer,1, 2048, pageFile);
		writeToSocket(buffer, writeSocket, bytesRead);
		if (ferror(pageFile))
		{
			if (errno == EINTR)
				continue;
			else
				error("Datei konnte nicht gelesen werden.");
		}
	}
	fclose(pageFile);
}

void loop(int serverSocket)
{
	char buffer[2048];
	char pathBuffer[100];
	int clientSocket;
	struct sockaddr_in clientAddress;
	socklen_t clientLength;
	fd_set readableSocketsSet;
	FD_ZERO(&readableSocketsSet);	
	FD_SET(serverSocket,&readableSocketsSet);
	int max = serverSocket;
	int sockets[MaxConnections];
	for (int i = 0; i < MaxConnections; i++)
		sockets[i] = -1;
	while (1)
	{
		struct fd_set readableSocketsSetCopy;
		FD_ZERO(&readableSocketsSetCopy);
		FD_COPY(&readableSocketsSet, &readableSocketsSetCopy);
		if (select(max + 1, &readableSocketsSetCopy, NULL, NULL, NULL) < 0)
			error("Fehler bei select():");
		if (FD_ISSET(serverSocket, &readableSocketsSetCopy))
		{
			clientLength = sizeof(clientAddress);
			clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength);
			if (clientSocket < 0) 
				error("Konnte Verbindung nicht akzeptieren:");
			for (int i = 0; i < MaxConnections; i++)
				if (sockets[i] == -1)
					sockets[i] = clientSocket;
			FD_SET(clientSocket, &readableSocketsSet);
			FD_SET(clientSocket, &readableSocketsSetCopy);
			if (clientSocket > max)
				max = clientSocket;
		}
		for (int i = 0; i < MaxConnections; i++)
		{
			clientSocket = sockets[i];
			if (clientSocket == -1)
				continue;
			if (FD_ISSET(clientSocket, &readableSocketsSetCopy))
			{
				bzero(buffer,2048);
				if (read(clientSocket, buffer, sizeof(buffer)) < 0)
					error("Konnte Request nicht lesen.");
				printf("Client:%s\n",buffer);
				parseRequest(buffer,pathBuffer);
				writeResponse(clientSocket, pathBuffer);
				printf("Parser:%s\n",pathBuffer);
				max = serverSocket;
				for (int i = 0; i < MaxConnections; i++)
				{
					if (sockets[i] == clientSocket)
						sockets[i] = -1;
					if (sockets[i] > max)
						max = sockets[i];
				}
				FD_CLR(clientSocket, &readableSocketsSet);
				close(clientSocket);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	loop(initialize(80));
	return 0;
}


