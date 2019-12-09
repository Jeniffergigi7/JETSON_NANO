#include "../include/Command.h"

CommandNode * commandHead = NULL;

unsigned long nextCommandId = 1;

// used for the likely scenario that the user sends a command to place new command
// after the currently executing command, and the currently executing command finishes
// before the new command arrives
unsigned long lastCommandExecuted = 0;

CommandNode * CreateNewCommandNode(CmdMsg * cmdMsg,
			           CommandNode * nextCommand)
{
	CommandNode * temp;

	temp = malloc(sizeof(CommandNode));
	temp->commandId = nextCommandId++;
	temp->commandType = cmdMsg->commandType;
	temp->position.latitude = cmdMsg->position.latitude;
	temp->position.longitude = cmdMsg->position.longitude;
	temp->nextCommand = nextCommand;
	
	return temp;
}

int InsertCommand(CmdMsg * cmdMsg, Message * message)
{
	CommandNode * temp;
	int insertionType = HEAD_INSERT;
	unsigned int currentPosition = 1;

	// if list is empty or the command we want to place the new command at was just popped
	if (NULL == commandHead || lastCommandExecuted == cmdMsg->previousCommandId) {
		commandHead = CreateNewCommandNode(cmdMsg, commandHead);
	} else {
		temp = commandHead;
		while (NULL != temp->nextCommand && temp->commandId != cmdMsg->previousCommandId) {
			temp = temp->nextCommand;
		}
		
		temp->nextCommand = CreateNewCommandNode(cmdMsg, temp->nextCommand);
		insertionType = NON_HEAD_INSERT;
	}

	if (HEAD_INSERT == insertionType) {
		message->messageType = DirectionMessage;
		message->source = TX2Master;
		message->directionMsg.position.latitude = commandHead->position.latitude;
		message->directionMsg.position.longitude = commandHead->position.longitude;
	}

	return insertionType;
}

int GetNextCommand(Message * message)
{
	CommandNode * temp;
	if (NULL == commandHead) {
		printf("\n\nCOMMAND LIST EMPTY\n\n");
		message->destination = TX2Master;
		return 0;
	} else {
		temp = commandHead;
		commandHead = commandHead->nextCommand;

		lastCommandExecuted = temp->commandId;
		free(temp);

		message->source = TX2Master;

		if (NULL != commandHead) {
			if (PositionCommand == commandHead->commandType) {
				message->destination = TX2Nav;
				message->messageType = DirectionMessage;

				message->directionMsg.position.latitude = commandHead->position.latitude;
				message->directionMsg.position.longitude = commandHead->position.longitude;
			} else if (CameraCommand == commandHead->commandType) {
				message->destination = TX2Cam;
				message->messageType = CamMessage;
			}
			
			return 1;
		}
	}
	return 0;
}

int DeleteCommand(CmdMsg * cmdMsg, Message * message)
{
	CommandNode * temp;
	CommandNode * nodeToDelete;
	temp = commandHead;

	while (NULL != temp->nextCommand && cmdMsg->commandId != temp->nextCommand->commandId) {
		temp = temp->nextCommand;
	}

	if (cmdMsg->commandId == temp->nextCommand->commandId) {
		// if head has changed, notify nav of the new destination
		// this will need to be modified to account for camera functionality
		if (temp == commandHead) {
			message->source = TX2Master;
			message->messageType = DirectionMessage;
			message->directionMsg.position.latitude = commandHead->nextCommand->position.latitude;
			message->directionMsg.position.longitude = commandHead->nextCommand->position.longitude;
		}

		nodeToDelete = temp->nextCommand;
		temp->nextCommand = temp->nextCommand->nextCommand;
		free(nodeToDelete);
	}
}

void FlushCommands()
{
	CommandNode * temp;

	while (NULL != commandHead) {
		temp = commandHead;
		commandHead = commandHead->nextCommand;
		free(temp);
	}

	lastCommandExecuted = 0;
	nextCommandId = 1;
}

void PrintCommands()
{
	CommandNode * temp;
	temp = commandHead;

	while (temp != NULL) {
		printf("commandId = %ld\n", temp->commandId);
		printf("lat %f lon %f\n\n", temp->position.latitude, temp->position.longitude);
		temp = temp->nextCommand;
	}
}
