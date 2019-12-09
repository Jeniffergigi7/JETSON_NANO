#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/stat.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include "include/Messages.h"

int sock;

void ReadFromSocket()
{
	int i;
	Message messageIn;
	read(sock, &messageIn, sizeof(messageIn));

	if (messageIn.messageType == CANMessage)
	{
		printf("\rCAN Message - ");
		printf("SId %X - ", messageIn.canMsg.SId);
		for (i = 0; i < messageIn.canMsg.Bytes; i++)
			printf("%X", messageIn.canMsg.Message[i]);
		printf("\n\n\r");
	} 
	else if (messageIn.messageType == CamMessage)
	{
		printf("\n\rReceiving image..\n");
		char fileName[32];
                char command[15];
		char bytes[4096];
		int bytesToRead = 4096;
		int bytesRemaining = messageIn.camMsg.fileSize;

		memcpy(fileName, &messageIn.camMsg.fileLocation[13], 18);

		int imageFile = open(fileName, O_RDWR | O_CREAT);		

		if (imageFile <= 0) 
		{
			printf("error creating image\n");
		}

		printf("\n\rwriting file %s\n", fileName);

		while (bytesRemaining > 0)
		{
			read(sock, bytes, bytesToRead);
			write(imageFile, bytes, bytesToRead);
			bytesRemaining -= bytesToRead;
			if (bytesRemaining < bytesToRead) {
				bytesToRead = bytesRemaining;
			}
		};

		fchmod(imageFile, 444);
		close(imageFile);
		printf("\n\rFile received.\n\r");
	}
	else if (messageIn.messageType == OKMessage)
	{
		write(sock, &messageIn, sizeof(messageIn));
	}
}

int main(int argc, char ** argv)
{
	fd_set rdfs;
	int readFds[1];
	sock = atoi(argv[1]);
	readFds[0] = sock;

	SetupSetAndWait(readFds, 1);

	while(1)
	{
		if (SetAndWait(&rdfs, 1, 0) < 0) {
			printf("error\n");
		}

		if (FD_ISSET(sock, &rdfs)) {
			ReadFromSocket();
		}
	}
}
