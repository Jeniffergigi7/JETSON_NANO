/**
 * @file tx2_comm_node.c
 * @author Patrick Henz
 * @date 4-8-2019
 * @brief Communication node for TX2.
 * @details FILL ME IN PLEASE. 
**/

#include <stdio.h>
#include "../include/Messages.h"
#include "../include/CommController.h"
#include <unistd.h>
#include <signal.h>

#define NOTHING_FROM_CLIENT_TIMEOUT 300 
#define NOTHING_FROM_CLIENT_AFTER_SOCKET_CHECK_TIMEOUT 60

#define DEBUG /**< This compiles the program for debug mode. There are certain macros and print
		   statements that are included when DEBUG is defined. Comment out for non-debug
		   commpilation. */

#define SOCKET_OK "OK"
#define PORT 5000 /**< Port number used for communication */
#define ADD_POR_REUSE (SO_REUSEADDR | SO_REUSEPORT)

#ifdef DEBUG
#define WASD_PRESS(c) (c == 'w' || c == 'W' ||\
		       c == 'a' || c == 'A' ||\
		       c == 's' || c == 'S' ||\
		       c == 'd' || c == 'D') /**< Debug macro to check incoming key presses from
		       				  the controller.c program. */
#endif

/*
int WaitForClient(int setupSocket, int currentSocket, sockaddr_in address, int addressLength)
{
	
}*/


void ALARMHandler()
{
	//alarmTimeRemaining = alarm(0);
}

int main(int argc, char ** argv)
{
#ifdef DEBUG
	printf("staring communication node\n");
#endif
	fd_set rdfs;
	int status;
	int tcpSocket;
	int oldTcpSocket;
	int masterRead;
	int masterWrite;
	int nbytes;
	int readFds[2];
	int i;
	int killMessageReceived;
	unsigned int alarmTimeRemaining;
	unsigned int socketCheckSent;
	unsigned int wakeCount;

	Message commInMessage;
	Message commOutMessage;

	struct sigaction myAlarm;
	//myAlarm.sa_handler = ALARMHandler;
	myAlarm.sa_flags = 0;
	sigaction(SIGALRM, &myAlarm, NULL);

	if (argc != 3)
	{
		printf("Error starting Comm Node\n");
		return -1;
	}

	masterRead = atoi(argv[1]);
	masterWrite = atoi(argv[2]);

	tcpSocket = InitializeComm(PORT);
	oldTcpSocket = tcpSocket;

	readFds[0] = masterRead;
	readFds[1] = tcpSocket;

	SetupSetAndWait(readFds, 2);
	//SetupSIGIOHandler(readFds,2);

	alarmTimeRemaining = NOTHING_FROM_CLIENT_TIMEOUT;
	socketCheckSent = 0;

	killMessageReceived = 0;
	wakeCount = 0;

	//  main while loop
	while(!killMessageReceived)
	{
		if (SetAndWait(&rdfs, 1, 0) < 0 ) {
			printf("SET AND WAIT ERROR COMM\n");
		}		

		// TODO GET RID OF MAGIC NUMBER
		for (i = 0; i < 2; i++) {
			if (!FD_ISSET(readFds[i], &rdfs)) {
				continue;
			}

			if (readFds[i] == tcpSocket)
			{
				wakeCount = 0;

				CommRead(&commInMessage);

				//  the only time this should happen if the client sent a 
				//  request to the TX2 with the TX2 sending a socket check at 
				//  the same time. We should have already processed the 
				//  request from the client. See below.
				if (commInMessage.messageType == OKMessage)
				{
					socketCheckSent = 0;
					printf("OK received, reseting alarm value\n");
				}
				else if (commInMessage.messageType == ClientDisconnect)
				{
					printf("client requsting disconnect\n");
					tcpSocket = EstablishSocket();
					ModifySetAndWait(oldTcpSocket, tcpSocket);
					oldTcpSocket = tcpSocket;
				}
				else
				{
					commInMessage.source = TX2Comm;
					write(masterWrite, &commInMessage, sizeof(commInMessage));
				}
			}
			else if (readFds[i] == masterRead)
			{
				read(masterRead, &commOutMessage, sizeof(commOutMessage));
				printf("reading from master\n");

				if (commOutMessage.messageType == CamMessage) 
				{
					printf("send image..\n");
					CommImageWrite(&commOutMessage);
				}
				else if (commOutMessage.messageType == KillMessage)
				{
					killMessageReceived = 1;
					CloseSocket();
					close(masterWrite);
					close(masterRead);
				}
				else
				{
					printf("here for some reason\n");
					printf("message is from %d\n", commOutMessage.source);
					CommWrite(&commOutMessage);
				}
			}
		}

		wakeCount++;

		// we need to set the alarm to 0 afterwards. If we woke up from a pipe and 
		// we are in the middle of a read when the alarm goes off, this process will die
		// if alarmTimeRemainging is 0, OK was never received
		if (wakeCount == NOTHING_FROM_CLIENT_AFTER_SOCKET_CHECK_TIMEOUT && socketCheckSent == 1)
		{
			printf("client disconnected\n");
			tcpSocket = EstablishSocket();
			ModifySetAndWait(oldTcpSocket, tcpSocket);
			oldTcpSocket = tcpSocket;
			wakeCount = 0;
			socketCheckSent = 0;
		} 
		else if (wakeCount == NOTHING_FROM_CLIENT_TIMEOUT)
		{
			printf("checking socket\n");
			SocketCheck();
			wakeCount = 0;
			socketCheckSent = 1;
		}
	}

	printf("killing comm node\n");
	return 0;
}
