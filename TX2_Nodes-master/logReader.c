/**
 * @file controller.c
 * @author Patrick Henz
 * @date 4-11-2019
 * @brief Program serves as a basic remote control for TX2 rover.
 * @details This program serves as a basic remote control for the TX2 rover through the
 *          use of tcp sockets. At the moment it only sends basic move commands via WASD
 *	    style key strokes. Pressing any other key will exit out of the program. This 
 * 	    program is not a TX2 node.
 *
 *	    ---key strokes-- (case-insensitive)
 *	    W - forward
 *	    A - turn left
 *	    D - turn right
 *          S - backwards
 */

#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "include/Messages.h"
#include <errno.h>

#define F_SETSIG 10

#define PORT 5000 

#define SCHOOL "199.17.28.95"
#define HOME "192.168.1.4"

#define WASD_PRESS(c) (c == 'w' || c == 'W' ||\
		       c == 'a' || c == 'A' ||\
		       c == 's' || c == 'S' ||\
		       c == 'd' || c == 'D')

void SIGIOHandler(int signum, siginfo_t * ptr1, void * ptr2)
{
	printf("test\n");
}

int main(int argc, char const *argv[]) 
{ 
	int sock = 0;
	int i;
	int bytes;
	int fd;

	Message message;

	sleep(1);

	fd = open("logFile.txt", O_RDWR);

	printf("fd = %d\n", errno);


	struct sigaction NewIo;
	NewIo.sa_sigaction = SIGIOHandler;
	NewIo.sa_flags = SA_SIGINFO;

	sigaction(SIGIO, &NewIo, NULL);

	fcntl(fd, F_SETFL, FASYNC);
	fcntl(fd, F_SETSIG, SIGIO);

	printf("Starting return window\n");
	
	do
	{
		while (read(fd, &message, sizeof(message)) > 0)
		{
			printf("CAN message :\n");
			printf("---------------------------\n");
			printf("SId : %X\n", message.canMsg.SId);
			for (i = 0; i < message.canMsg.Bytes; i++)
				printf("%X", message.canMsg.Message[i]);
			printf("\n\n");
		}

		usleep(100);
	} while (1);
	

	return 0; 
} 

