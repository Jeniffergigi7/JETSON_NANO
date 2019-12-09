#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define PARAMETERS_FILE "../Parameters.txt"

typedef struct parameters {
        float dotProductThreshold;
        float distanceToGoThreshold;
        float distanceFromStartThreshold;
	float distanceFromPreviousThreshold;
        float angleToTurnThreshold;
	float turningWeight;
	float turningAngle;
	float multiTurnThreshold;
        int   sideDotProductValueCount;
	int   centerDotProductValueCount;
	int   usingGps;
	int   manual;
} Parameters;

int GetParameters(char * fileName, Parameters * parameters);

void PrintParameters(Parameters * parameters);

#endif
