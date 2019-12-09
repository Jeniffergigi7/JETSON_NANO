/**
 * TO COMPILE THIS YOU MUST LINK WITH MATH LIBRARY
 * gcc [flags] <files> -lm
 * WITHOUT -lm YOU WILL GET UNDEFINED REFERENCE ERROR
**/

#include <math.h>
#include <stdio.h>

#define LAT_DIFF(p1, p2) (p1.latitude - p2.latitude)
#define LON_DIFF(p1, p2) (p1.longitude - p2.longitude)
#define SQUARE(n) pow((n), 2)

#define PREV_TO_ORIG(pos, prev) (pos.latitude -= prev.latitude);\
				(pos.longitude -= prev.longitude)


typedef struct _Position {
	double latitude;
	double longitude;
} Position;

double Distance(Position position1, Position position2)
{
	return sqrt(SQUARE(LAT_DIFF(position1, position2)) 
		  + SQUARE(LON_DIFF(position1, position2)));
}

double DegreeTurn(double a, double b, double c)
{
	float sign;
	float cosValue; 
	float divSlope;

	cosValue = -(SQUARE(c) - SQUARE(a) - SQUARE(b))/(2.0f*a*b);

	return acos(cosValue);
}

double NeedToTurn(Position currentPosition, 
		 Position previousPosition, 
		 Position destinationPosition)
{
	double dTraveled, dPreviousToDestination, dCurrentToDestination;
	double degreesToTurn;
	double slope, pointOnLine;
	double sign;

	PREV_TO_ORIG(currentPosition, previousPosition);
	PREV_TO_ORIG(destinationPosition, previousPosition);
	PREV_TO_ORIG(previousPosition, previousPosition);

	dTraveled = Distance(currentPosition, previousPosition);
	dPreviousToDestination = Distance(previousPosition, destinationPosition);
	dCurrentToDestination = Distance(currentPosition, destinationPosition);

	slope = currentPosition.latitude/currentPosition.longitude;	
	printf("slope = %f\n", slope);

	pointOnLine = slope*destinationPosition.longitude;

	if (slope < 0) {
		sign = (destinationPosition.latitude > pointOnLine)?(1.0f):(-1.0f);
	} else {
		sign = (destinationPosition.latitude > pointOnLine)?(-1.0f):(1.0f);
	}

	return sign*(1.570796 - DegreeTurn(dTraveled, dCurrentToDestination, dPreviousToDestination));
}

int main(int argc, char ** argv)
{
	Position position1 = {.latitude = 1, .longitude = -3};
	Position position2 = {.latitude = 2, .longitude = -4};
	Position position3 = {.latitude = 2, .longitude = -1};

	double angle = NeedToTurn(position2, position1, position3);

	printf("angle = %f\n", angle);
	
	return 0;
}	
