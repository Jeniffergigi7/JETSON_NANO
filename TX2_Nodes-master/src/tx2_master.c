/**
 * @file tx2_master.c
 * @author Patrick Henz
 * @date 12-1-2019 
 * @brief Master node for TX2.
 * @details FILL ME IN PLEASE. 
 */

#define DEBUG /**< Used to compile the master node in debug mode. */
#define IS_CAN_NODE(pid) (pid == 0)
#define canNodeRead canNodeReadPipe[0]
#define canNodeWrite canNodeWritePipe[1]

#define IS_COMM_NODE(pid) (pid == 0)
#define commNodeRead commNodeReadPipe[0]
#define commNodeWrite commNodeWritePipe[1]

#define IS_CAM_NODE(pid) (pid == 0)
#define camNodeRead camNodeReadPipe[0]
#define camNodeWrite camNodeWritePipe[1]

#define IS_NAV_NODE(pid) (pid == 0)
#define navNodeRead navNodeReadPipe[0]
#define navNodeWrite navNodeWritePipe[1]

#define IS_GPS_NODE(pid) (pid == 0)
#define gpsNodeRead gpsNodeReadPipe[0]
#define gpsNodeWrite gpsNodeWritePipe[1]

#define IS_GYRO_NODE(pid) (pid == 0)
#define gyroNodeRead gyroNodeReadPipe[0]
#define gyroNodeWrite gyroNodeWritePipe[1]

#include "../include/Messages.h"
#include "../include/Command.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

#define CHILD_COUNT 6

//int CommMessage; /**< Flag notifies master node that there is a message from the communication node. */

unsigned int sigInt;

void SIGINTHandler(int signum)
{
	//sigInt = 1;
	printf("SIGINT RECEIVED\n");
}

int main(int argc, char ** argv)
{
#ifdef DEBUG
	printf("starting master node\n");
#endif	
	fd_set rdfs;
	struct timeval timeout;

	int readPipes[CHILD_COUNT];
	int writePipes[CHILD_COUNT];

	int commNode;
	int commNodeReadPipe[2];
	int commNodeWritePipe[2];

	int camNode;
	int camNodeReadPipe[2];
	int camNodeWritePipe[2];

	int status;
	int killMessageReceived;
	int i;

	char param1[16];
	char param2[16];

	Message message;
	unsigned int messageOkToSend;
	
	
	if(pipe(commNodeReadPipe) | pipe(commNodeWritePipe))
	{
		printf("Error creating CommNode Pipes\n");
		return -1;
	}

	commNode = fork();

	if (IS_COMM_NODE(commNode))
	{
		close(commNodeReadPipe[READ]);
		close(commNodeWritePipe[WRITE]);
		sprintf(param1, "%d", commNodeWritePipe[READ]); // Comm read
		sprintf(param2, "%d", commNodeReadPipe[WRITE]); // Comm write 
		execl("./tx2_comm_node", "tx2_comm_node", param1, param2, (char *)NULL);
	}
	else
	{
		close(commNodeReadPipe[WRITE]);
		close(commNodeWritePipe[READ]);
	}	

	if(pipe(camNodeReadPipe) | pipe(camNodeWritePipe))
	{
		printf("Error creating CamNode Pipes\n");
		return -1;
	}

	camNode = fork();

	if (IS_CAM_NODE(camNode))
	{
		close(camNodeReadPipe[READ]);
		close(camNodeWritePipe[WRITE]);
		sprintf(param1, "%d", camNodeWritePipe[READ]); // Cam read
		sprintf(param2, "%d", camNodeReadPipe[WRITE]); // Cam write 
		execl("./tx2_cam_node", "tx2_cam_node", param1, param2, (char *)NULL);
	}
	else
	{
		close(camNodeReadPipe[WRITE]);
		close(camNodeWritePipe[READ]);
	}	

	
	readPipes[TX2Comm] = commNodeRead;
	readPipes[TX2Cam] = camNodeRead;
	SetupSetAndWait(readPipes, 2);

	writePipes[TX2Comm] = commNodeWrite;
	writePipes[TX2Cam] = camNodeWrite;

	// Once a little more work has been done on the other modules, will set this up to have master
	// send a exit message to the other nodes, who will then leave their while loops and terminate
	//struct sigaction NewIo;
	//NewIo.sa_handler = SIGINTHandler;
	//NewIo.sa_flags = 0x0;

	//sigaction(SIGINT, &NewIo, NULL);

	killMessageReceived = 0;	

	while(!killMessageReceived) {
		if (SetAndWait(&rdfs, 1, 0) < 0) {
			printf("\n\nERROR\n\n)");
		}		

		for (i = 0; i < CHILD_COUNT; i++) {

			if (!FD_ISSET(readPipes[i], &rdfs)) {
				continue;
			}

			read(readPipes[i], &message, sizeof(message));

		   if (message.messageType == KillMessage) {
				killMessageReceived = 1;
				break;
			} else if (CommandMessage == message.messageType && 
				 message.destination == TX2Master) {
				printf("\n\nPOPING COMMAND QUEUE\n\n");
				messageOkToSend = GetNextCommand(&message);
				if (!messageOkToSend) {
					continue;
				}	
			} else if (CommandMessage == message.messageType && message.source == TX2Comm) {
				switch (message.cmdMsg.commandOperation) {
					case Create:
						InsertCommand(&message.cmdMsg, &message);
						break;
					case Update:
						// this will be implemented as an update and delete
						break;
					case Delete:
						DeleteCommand(&message.cmdMsg, &message);
						break;
					default:
						printf("unkown command message operation");
						break;
				}
				PrintCommands();
				if (message.destination != TX2Nav) {
					continue;
				}
			}

			write(writePipes[message.destination], &message, sizeof(message));
		}
	}


	printf("killing child processes\n");

	for (i = 0; i < 5; i++) {
		memset(&message, 0, sizeof(message));
		message.messageType = KillMessage;
		write(writePipes[i], &message, sizeof(message));
		printf("closing %d\n", i);
		close(readPipes[i]);
		close(writePipes[i]);
	}	

	printf("master signing off...\n");

	return 0;
}
