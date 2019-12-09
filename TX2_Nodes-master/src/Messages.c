#include "../include/Messages.h"

int * fileDescriptors;
int maxFileDescriptor;
int fdCount;
//struct timeval timeout;
struct timespec timeout;

void SetupSetAndWait(int * fd, int count)
{
	int i;

	fileDescriptors = fd;	
	fdCount = count;

	maxFileDescriptor = 0;

	for (i = 0; i < count; i++) {
		if (maxFileDescriptor < fd[i]) {
			maxFileDescriptor = fd[i];
		}
	}
}

int ModifySetAndWait(int oldFd, int newFd)
{
	int i;
	int found = 0;

	for (i = 0; i < fdCount; i++) {
		if (fileDescriptors[i] == oldFd) {
			fileDescriptors[i] = newFd;
			found = 1;
			break;
		}
	}

	if (found && newFd > maxFileDescriptor) {
		maxFileDescriptor = newFd;
	} else if (!found) {
		return -1;
	}

	return 0;
}

void CopyMessage(Message * destination, Message * source)
{
	memcpy(destination, source, sizeof(Message));
}

int SetAndWait(fd_set * rdfs, long seconds, long nanoSeconds)
{
	int i;
	
	FD_ZERO(rdfs);

	for (i = 0; i < fdCount; i++) {
		FD_SET(fileDescriptors[i], rdfs);
	}

	timeout.tv_sec  = seconds;
	timeout.tv_nsec = nanoSeconds;

	return pselect(maxFileDescriptor + 1, rdfs, NULL, NULL, &timeout, NULL);	
}
