/**
 * @file Messages.h
 * @author Patrick Henz
 * @date 4-28-2019
 * @brief Header file for Messages library.
 * @details Header file for Messages library. #Messages.h provides a convenient
 * 	    message struct that contains a union of all possible messages that
 * 	    can be passed around in the system. This library also provides wrapper
 *	    functions for the pselect() function (see select man page for additional
 *	    information). Originally, a process would be paused and signals were used
 *	    to wake a process up to read in a new message. That is NOT a good way to 
 *	    multiple processes communicating with one another, especially if communication
 *	    is asynchronous. If there is an incoming signal while a process is in a
 *	    signal handler, the new signal is lost. I didn't find a convenient wawy around
 *          this. Also, syscalls like read() and write() don't like being interrupted.
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h> 
#include <sys/socket.h>
#include <sys/uio.h> 
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef F_SETSIG
#define F_SETSIG 10
#endif

#define MESSAGE_AVAILABLE(fd) (-1 != (fd))

typedef enum opMode
{
	Automatic,
	Manual
} OpMode; /**< Opeartion States */

typedef enum cameraFunc
{
	SemanticSegmentation,
	ImageClassification
} CameraFunc;

typedef enum nodeNames {
	TX2Comm    = 0,
	TX2Can     = 1,
	TX2Cam     = 2,
	TX2Nav     = 3,
	TX2Gps     = 4,
	TX2Gyro    = 5,
	Controller = 6,
	TX2Master  = 7
} NodeName; /**< Enums used for message routing functionality. Values are hardcoded to make use of array indexing */

typedef enum messageTypes {
	TCPMessage,
	CamMessage,
	DirectionMessage,
	OKMessage,
	RestartMessage,
	ClientDisconnect,
	SharedMemory,
	OperationMode,
	ParametersMessage,
	KillMessage,
	CalibrationCompleteMessage,
	CommandMessage,
} MessageTypes; /**< Enum for all possible message types in the system */

///////////////////////////////
//  File Descriptor Structs  //
///////////////////////////////

typedef struct _FileDescriptors FileDescriptors;

struct _FileDescriptors {
	int fd;
	FileDescriptors * next;
}; /**< Struct for file descriptor queue (linked-list). Queue is a FIFO, first filedescriptor that generated a signal is the first out. */

/////////////////////////////
//  Message Union/Structs  //
/////////////////////////////

typedef struct opModeMsg {
	OpMode opMode;
} OpModeMsg;

typedef struct camMsg {
	int ready;
	CameraFunc cameraFunc;
	int fileSize;
	char fileLocation[32];
} CamMsg; /**< Camera message. Used primarily to pass location of image to Comm node. */

typedef struct shMem {
	int width;
	int height;
} ShMem; /**< Shared memory struct for Cam-to-Nav node. Used to give Nav node dimensions of shared memory. */


/////////////////////////////////////////////////
//	Command List Typedefs TODO	       //
/////////////////////////////////////////////////
typedef enum _CommandOperation {
	Create,
        Delete,
        Update,
	Flush
} CommandOperation; /**< Command queue is a linked-list. Commands can be Created, Deleted, Updated, or the Queue can be flushed. */

typedef enum _CommandType {
	PositionCommand,
	CameraCommand
} CommandType; /**< Specifies which node the command is for. */

typedef struct _CmdMsg {
	unsigned long commandId;
	CommandType commandType;
	CommandOperation commandOperation;
	unsigned long previousCommandId;
	Position position;
} CmdMsg; /**< This struct exists within #Message. Contains all information needed to perform an action on the Command queue. */


typedef struct okMsg {
	char message[32];
} OkMsg; /**< OK Message */

typedef struct message {
	MessageTypes messageType;
	NodeName source;
	NodeName destination;
	union {
		DirectionMsg directionMsg;
		CanMsg canMsg;
		CamMsg camMsg;
		ShMem shMem;
		OkMsg okMsg;
		OpModeMsg opModeMsg;
		GpsMsg gpsMsg;
		CmdMsg cmdMsg;
	};
} Message; /**< Message struct */

void CopyMessage(Message * destination, Message * source);

/**
 * @brief SetupSetAndWait() helps support pselect() functionality.
 * @details This function helps support pselect() functionality. It stores a copy
 * 	    of the array which was passed as a parameter, determines which file
 *	    descriptor is the largest, and sets gets the process ready to call pselect().
 * @param fd A pointer to the array of file descriptors that will be read from.
 * @param count The number of file descriptors in fd.
 * @pre Assumes fd has been populated correctly and count matches the number of values in fd.
 * @post Setup is complete, the wrapper function #SetAndWait() can now be called.
**/
void SetupSetAndWait(int * fd, int count);

/**
 * @brief Overwrites old file descriptor with new file descriptor for #SetupAndWait().
 * @details Overwrites old file descriptor with new file descriptor for #SetupAndWait(). 
 *	    in the scenario that a new file descriptor is needed to replace an old one,
 *          i.e. the tx2_comm_node needing to establish a new socket with controller, this
 *	    function overwrites the old file descriptor sets the max file descriptor if the
 *	    new one is larger. 
 *	    <br>
 *	    <center><b>NOTE: This does overwrite the array passed in whe #SetupSetAndWait() is
 *	    first called. Just be aware of this</b></center>
 * @param oldFd The old file descriptor to be replaced.
 * @param newFd The new file descriptor.
 * @return Returns -1 if the old file descriptor wasn't found. Returns 0 if success.
 * @pre Expects #SetupSetAndWait() to have been called prior to ModifySetAndWait(). 
 * @post The old file descriptor will have been replaced with the new file descriptor.
**/
int ModifySetAndWait(int oldFd, int newFd);

/**
 * @brief Wrapper function which calls pselect().
 * @details SetAndWait() performs some basic setup before calling pselect(). This function
 *	    returns under 2 conditions; a file descriptor is available to be read, or a timeout.
 * @param rdfs The address of the fd_set read file descriptors variable.
 * @param seconds pselect() timeout in seconds.
 * @param nanoSeconds pselect() timeout in nano seconds.
 * @return The return value is the same value returned from pselect(). If a timeout occurs,
 *	   no file descriptors are available and 0 is returned. If file descriptors are 
 *	   available, the number of file descriptors is returned. -1 is returned on error.
 * @pre Assumes #SetupSetAndWait() was called prior to calling SetAndWait().
**/
int SetAndWait(fd_set * rdfs, long seconds, long nanoSeconds);

#endif
