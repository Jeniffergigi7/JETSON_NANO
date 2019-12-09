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
#include <unistd.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h> 
#include <arpa/inet.h>
#include <string.h> 
#include <fcntl.h>
#include <errno.h>
#include "include/Messages.h"

//  /usr/src/linux-headers-4.15.0-46-generic/include/uapi/asm-generic/errno-base.h 

#define RING_BUFFER_SIZE 8

#define F_SETSIG 10

#define PORT 5000 

#define SCHOOL "199.17.28.95"
#define SCHOOL_WIFI "10.101.35.50"
#define HOME "192.168.1.2"

#define WASD_PRESS(c) (c == 'w' || c == 'W' ||\
		       c == 'a' || c == 'A' ||\
		       c == 's' || c == 'S' ||\
		       c == 'd' || c == 'D')

#define DIR_PRESS(c) (c == '1' || c == '2' ||\
		      c == '3' || c == '4')

#define COPY_POS(d, s) d.latitude = s.latitude;\
		       d.longitude = s.longitude

#define PARAMETERS(c) ('p' == (c) || 'P' == (c))

#define KILL(c) ('k' == (c) || 'K' == (c))

Position p1 = { .latitude = 45.550721f, .longitude = -94.151741f };
Position p2 = { .latitude = 45.551082f, .longitude = -94.151746f };
Position p3 = { .latitude = 45.551488f, .longitude = -94.151698f };
Position p4 = { .latitude = 45.551071f, .longitude = -94.151232f };

Position p5 = { .latitude = 45.543357f, .longitude = -94.151385f };
Position p6 = { .latitude = 45.543117f, .longitude = -94.150917f };
Position p7 = { .latitude = 45.542591f, .longitude = -94.150917f };
Position p8 = { .latitude = 45.542295f, .longitude = -94.150085f };

int main(int argc, char const *argv[]) 
{ 
	int status, index, bytes, i;
	struct sockaddr_in address; 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char param1[16];
	int opt = 1;
	int child1, child2;
	int exit = 0;
	unsigned long previousCommandId;
	

	OpMode opMode = Manual;

	char keyPress;

	printf("Starting Controller\n");

	Message message;

	memset(&message, 0, sizeof(message));

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	memset(&serv_addr, '0', sizeof(serv_addr)); 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	// 199.17.28.95
	//if(inet_pton(AF_INET, SCHOOL_WIFI, &serv_addr.sin_addr)<=0) 
	if(inet_pton(AF_INET, HOME, &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	//sock = 6;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	
	if((child1 = fork()) == 0)
	{
		sprintf(param1, "%d", sock);
		execl("./logWriter", "logWriter", param1, (char*) NULL);
	}

	sleep(1);

	system("/bin/stty raw");

	printf("starting \n");
	do
	{
		status = read(1, &keyPress, sizeof(keyPress));

		printf("\r");
		if (WASD_PRESS(keyPress))
		{
			message.messageType = CANMessage;
			message.destination = TX2Nav;
			message.canMsg.SId = 0x123;
			message.canMsg.Bytes = 1;

			if (keyPress == 'w')
				message.canMsg.Message[0] = 2;
			else if (keyPress == 'a')
				message.canMsg.Message[0] = 1;
			else if (keyPress == 'd')
				message.canMsg.Message[0] = 0;
			else if (keyPress == 's')
				message.canMsg.Message[0] = 3;
			else
				message.canMsg.Message[0] = 4;

			status = write(sock ,&message, sizeof(message));
			//printf("Status = %d\n", status);
		}
		else if (keyPress == 'c' || keyPress == 'C')
		{
			printf("cam\n");
			message.destination = TX2Cam;
			message.messageType = CamMessage;

			//for (i = 0; i < sizeof(message); i++) {
			//	printf("%X", *(((char *)&message)+i));
			//}

			write(sock, &message, sizeof(message));
		}
		else if (keyPress == 'm' || keyPress == 'M')
		{
			message.destination = TX2Nav;
			message.messageType = OperationMode;
			if (opMode == Manual) {
				opMode = Automatic;
				message.opModeMsg.opMode = Automatic;
			} else {
				opMode = Manual;
				message.opModeMsg.opMode = Manual;
			}
			write(sock, &message, sizeof(message));
		}
		else if (DIR_PRESS(keyPress))
		{
			message.messageType = DirectionMessage;
			message.destination = TX2Nav;
			if ('1' == keyPress)
			{
				COPY_POS(message.directionMsg.position, p1);
			}
			else if ('2' == keyPress)
			{
				COPY_POS(message.directionMsg.position, p2);
			}
			else if ('3' == keyPress)
			{
				COPY_POS(message.directionMsg.position, p3);
			}
			else if ('4' == keyPress)
			{
				COPY_POS(message.directionMsg.position, p4);
			}
			printf("going to lat = %.6f, lon = %.6f\n", 
				message.directionMsg.position.latitude,
				message.directionMsg.position.longitude);

			write(sock, &message, sizeof(message));
		}
		else if (PARAMETERS(keyPress))
		{
			message.messageType = ParametersMessage;
			message.destination = TX2Nav;
			write(sock, &message, sizeof(message));
		}
		else if (KILL(keyPress))
		{
			message.messageType = KillMessage;
			write(sock, &message, sizeof(message));
		}
		else if ('5' == keyPress)
		{
			message.messageType = CommandMessage;
			message.destination = TX2Master;
			message.cmdMsg.commandType = PositionCommand;
			message.cmdMsg.commandOperation = Create;
			message.cmdMsg.previousCommandId  = 0;

			COPY_POS(message.cmdMsg.position, p2);
			write(sock, &message, sizeof(message));
			message.cmdMsg.previousCommandId++;

			//usleep(500000);

			COPY_POS(message.cmdMsg.position, p1);
			write(sock, &message, sizeof(message));
			message.cmdMsg.previousCommandId++;

			//usleep(500000);

			COPY_POS(message.cmdMsg.position, p2);
			write(sock, &message, sizeof(message));
			message.cmdMsg.previousCommandId++;

			//usleep(500000);

			COPY_POS(message.cmdMsg.position, p3);
			write(sock, &message, sizeof(message));
			message.cmdMsg.previousCommandId++;

			//usleep(500000);
	
			COPY_POS(message.cmdMsg.position, p2);
			write(sock, &message, sizeof(message));
			message.cmdMsg.previousCommandId++;

			//usleep(500000);

			COPY_POS(message.cmdMsg.position, p4);
			write(sock, &message, sizeof(message));
			message.cmdMsg.previousCommandId++;
		}
		else if ('6' == keyPress)
		{
			message.messageType = CommandMessage;
			message.destination = TX2Master;
			message.cmdMsg.commandType = PositionCommand;
			message.cmdMsg.commandOperation = Create;
			message.cmdMsg.previousCommandId  = 0;

			COPY_POS(message.cmdMsg.position, p5);
			write(sock, &message, sizeof(message));
			message.cmdMsg.previousCommandId++;

			//usleep(500000);

			COPY_POS(message.cmdMsg.position, p6);
			write(sock, &message, sizeof(message));
			message.cmdMsg.previousCommandId++;

			//usleep(500000);

			COPY_POS(message.cmdMsg.position, p7);
			write(sock, &message, sizeof(message));
			message.cmdMsg.previousCommandId++;

			//usleep(500000);

			COPY_POS(message.cmdMsg.position, p5);
			write(sock, &message, sizeof(message));
		}
		else
		{
			message.messageType = ClientDisconnect;
			write(sock, &message, sizeof(message));
			exit = 1;
		}	
	} while (!exit);

	system("/bin/stty cooked");	

	kill(child1, SIGKILL);
	kill(child2, SIGKILL);

	shutdown(sock, SHUT_RDWR);

	close(sock);

	//printf("Hello message sent\n"); 
	//valread = read( sock , buffer, 1024); 
	//printf("%s\n",buffer ); 
	return 0; 
} 

