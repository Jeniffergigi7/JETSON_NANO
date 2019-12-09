#include "../include/SharedMem.h"

int segFd;
int angFd;
int posFd;

SharedMem * CreateSharedMemory(int size, SMType type)
{
	SharedMem * sharedMem;
	int memFd;

	// modes (3rd param) in usr/include/aarch64-linux-gnu/sys/stat.h
	switch (type) {
		case SegmentationData:
			memFd = segFd = shm_open(SHARED_SEG_NAME, O_CREAT | O_RDWR, S_IRWXU);
			break;
		case AngleData:
			memFd = angFd = shm_open(SHARED_ANG_NAME, O_CREAT | O_RDWR, S_IRWXU);
			break;
		case PositionData:
			memFd = posFd = shm_open(SHARED_POS_NAME, O_CREAT | O_RDWR, S_IRWXU);
	}


	if (memFd <= 0)
	{
		printf("error opening shared memory\n");
		return NULL;
	}

	int stat = ftruncate(memFd, size + sizeof(SharedMem));

	if (stat == -1)
	{
		printf("error truncating file\n");
		return NULL;
	}

	// see man page for protections and flags
	sharedMem = mmap(NULL, size + sizeof(SharedMem), PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);

	if (sharedMem == NULL)
	{
		printf("error mapping memory\n");
		return NULL;
	}

	return sharedMem;
}

SharedMem * OpenSharedMemory(int size, SMType type)
{
	SharedMem * sharedMem;
	int memFd;

	switch (type) {
		case SegmentationData:
			memFd = segFd = shm_open(SHARED_SEG_NAME, O_RDONLY, 0);
			break;
		case AngleData:
			memFd = angFd = shm_open(SHARED_ANG_NAME, O_RDWR, 0);
			break;
		case PositionData:
			memFd = posFd = shm_open(SHARED_POS_NAME, O_RDWR, 0);
			break;
	}

	if (memFd <= 0)
	{
		printf("error opening shared memory\n");
		return NULL;
	}

	switch (type) {
		case SegmentationData:
			sharedMem = mmap(NULL, size + sizeof(SharedMem), PROT_READ, MAP_SHARED, memFd, 0);
			break;
		case AngleData:
		case PositionData:
			sharedMem = mmap(NULL, size + sizeof(SharedMem), PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
			break;
	}

	if (sharedMem == NULL)
	{
		printf("error mapping memory\n");
		return NULL;
	}

	return sharedMem;
}

void CloseSharedMemory()
{
	close(segFd);
	close(angFd);
	close(posFd);
}
