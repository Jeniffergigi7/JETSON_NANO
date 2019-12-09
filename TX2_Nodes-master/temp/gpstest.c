#include <stdio.h>
#include "I2CGPS.h"

#define COMMAND "$PMTK353,1,0,0,0,0*"
#define WARMRST "$PMTK102*"
#define GPSONLY "$PMTK353,1,0,0,0,0*"
#define SEARCHGPSONLY "$PGCMD,229,1,0,0,0,0*"
#define FULLCOLD "$PMTK104*"
#define ACKTEST "$PMTK604,3*"
#define DEBUG "$PMTK299,1*"
#define CLEAR "$PMTK127*"
#define DIG4 "$PGCMD,220,0*"
#define START_LOGGING "$PMTK185,0*"
#define AIC "$PMTK286,1*"
#define SYNC "$PMTK255,1*"
#define TEST "$PMTK314,1,1,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,1,1,0*"
#define CMD_MODE "$PGCMD,380,7*"
#define OUTPUT "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*"
//#define OUTPUT "$PMTK250,3,3,9600*"
#define BAUD "$PMTK251,9600*"
#define ALWAYS_OUT "$PGCMD,235,4*"

int main(int argc, char ** argv)
{
	char buffer[255];
	int fd = I2CGPSOpen();

	if (fd <= 0) {
		printf("ERROR OPENING I2C FILE\n");
		return 0;
	}

	memset(buffer, 0, sizeof(buffer));

	printf("before read\n");

	//I2CGPSWrite(CMD_MODE);
	//I2CGPSWrite(SEARCHGPSONLY);
	//I2CGPSWrite(DIG4);
	////I2CGPSWrite(OUTPUT);
	//I2CGPSWrite(FULLCOLD);

	//I2CGPSWrite(SYNC);
	//I2CGPSWrite(GPSONLY);
	//I2CGPSWrite(OUTPUT);
	//
	//I2CGPSWrite(BAUD);
	//printf("%s\n", buffer);

	//sleep(60);

	//I2CGPSWrite(OUTPUT);	
	//I2CGPSWrite(ALWAYS_OUT);
	//I2CGPSWrite(START_LOGGING);

	while (1) {
		//I2CGPSWrite(ACKTEST);
		I2CGPSRead(buffer);

		sleep(2);
	}

	return 0;
}
