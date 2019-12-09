#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Messages.h"

#define SHARED_SEG_NAME "shared_nav_memory"
#define SHARED_ANG_NAME "shared_angle_memory"
#define SHARED_POS_NAME "shared_pos_memory"

#define SET_SHARED_ANGLE(mem, val) *((float *)((mem) + 1)) = (val);\
				   mem->dataAvailableFlag = 1

#define GET_SHARED_ANGLE(mem, val) while (0 == mem->dataAvailableFlag);\
				   (val) = *((float *)((mem) + 1));\
				   mem->dataAvailableFlag = 0

#define SET_SHARED_POSITION(mem, val) while (mem->currentlyBeingAccessed);\
				      ((Position *)((mem) + 1))->latitude = val.latitude;\
				      ((Position *)((mem) + 1))->longitude = val.longitude;\
				      mem->dataAvailableFlag = 1.0

#define GET_SHARED_POSITION(mem, val) do {\
					mem->currentlyBeingAccessed = 1;\
       				      	if (mem->dataAvailableFlag == 1) {\
				      		val.latitude = ((Position *)(mem + 1))->latitude;\
				      		val.longitude = ((Position *)(mem + 1))->longitude;\
						mem->dataAvailableFlag = 0;\
					}\
				        mem->currentlyBeingAccessed = 0;\
				      } while (0)


typedef enum _SMType {
	SegmentationData,
	AngleData,
	PositionData
} SMType;

typedef struct _SharedMem {
	unsigned int dataAvailableFlag : 1;
	unsigned int currentlyBeingAccessed : 1;
	unsigned int otherData : 30;
} SharedMem;

SharedMem * CreateSharedMemory(int size, SMType type);

SharedMem * OpenSharedMemory(int size, SMType type);

void CloseSharedMemory();

#endif
