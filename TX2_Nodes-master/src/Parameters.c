#include "../include/Parameters.h"

Parameters parameters;

void GetToNextValue(int fd) {
	char temp;

	read(fd, &temp, 1);

	while (!EOF || temp != ':') {
		read(fd, &temp, 1);
        }
}

float GetFloat(int fd)
{
	float floatTemp;
	char tempChar;
	float divider = 1.0f;
	float sign;

	floatTemp = 0.0f;

	read(fd, &tempChar, 1);	

	if ('-' == tempChar) {
		sign = -1.0f;
	} else {
		sign = 1.0f;
	}

	while (tempChar != '.') {
		floatTemp += (floatTemp*10) + (tempChar - '0');
		read(fd, &tempChar, 1);
	}

	read(fd, &tempChar, 1);

	while(tempChar != '\n') {
		divider *= 0.1f;
		floatTemp += divider*(tempChar - '0');
		read(fd, &tempChar, 1);
	}

	return (sign*floatTemp);
}

int GetInt(int fd)
{
	int tempInt;
	int sign;
	char tempChar;

	tempInt = 0;

	read(fd, &tempChar, 1);

	if (tempChar == '-') {
		sign = -1;
		read(fd, &tempChar, 1);
	} else {
		sign = 1;
	}

	while(tempChar != '\n') {
		tempInt = (tempInt * 10) + (tempChar - '0');
		read(fd, &tempChar, 1);	
	}

	return (sign*tempInt);
}

int GetParameters(char * fileName, Parameters * parameters)
{
	int fd;

	fd = open(fileName, O_RDONLY);

	if (fd <= 0) {
		return -1;
	}

	GetToNextValue(fd);
	parameters->distanceToGoThreshold = GetFloat(fd);
	GetToNextValue(fd);
	parameters->distanceFromStartThreshold = GetFloat(fd);	
	GetToNextValue(fd);
	parameters->angleToTurnThreshold = GetFloat(fd);
	GetToNextValue(fd);
	parameters->dotProductThreshold = GetFloat(fd);
	GetToNextValue(fd);
	parameters->sideDotProductValueCount = GetInt(fd);
	GetToNextValue(fd);
	parameters->centerDotProductValueCount = GetInt(fd);
	GetToNextValue(fd);
	parameters->turningWeight = GetFloat(fd);
	GetToNextValue(fd);
	parameters->distanceFromPreviousThreshold = GetFloat(fd);
	GetToNextValue(fd);
	parameters->turningAngle = GetFloat(fd);
	GetToNextValue(fd);
	parameters->multiTurnThreshold = GetFloat(fd);
	GetToNextValue(fd);
	parameters->usingGps = GetInt(fd);
	GetToNextValue(fd);
	parameters->manual = GetInt(fd);

	close(fd);

	return 0;
}

void PrintParameters(Parameters * parameters)
{
	printf("dotProductThreshold = %.4f\n", parameters->dotProductThreshold);
	printf("distanceToGoThreshold = %.4f\n", parameters->distanceToGoThreshold);
	printf("distanceFromStartThreshold = %.4f\n", parameters->distanceFromStartThreshold);
	printf("angleToTurnThreshold = %.4f\n", parameters->angleToTurnThreshold);
	printf("sideDotProductValueCount = %d\n", parameters->sideDotProductValueCount);
	printf("centerDotProductValueCount = %d\n", parameters->centerDotProductValueCount); 
	printf("turningWeight %.4f\n", parameters->turningWeight);
	printf("distanceFromPreviousThreshold %.4f\n", parameters->distanceFromPreviousThreshold);
	printf("turningAngle = %.6f\n", parameters->turningAngle);
	printf("multiTurnThres = %.6f\n", parameters->multiTurnThreshold);
	printf("usingGps = %s\n", (parameters->usingGps)?("True"):("False"));
	printf("manual = %s\n", (parameters->manual)?("True"):("False"));
}
