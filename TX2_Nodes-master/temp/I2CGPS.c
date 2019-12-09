#include "I2CGPS.h"

typedef struct i2c {
	unsigned int bytes;
	char gpsBuffer[255];
} I2C;

char nmeaBuffer[128];
int nmeaBufferSize;
int previousIndex;

int I2CFd;
I2C * i2cData;

int i2cWrite(I2C * i2c)
{
	int status;
        do {
		status = write(I2CFd, i2c->gpsBuffer, i2c->bytes);
        } while (-1 == status);
	usleep(5000);
	return status;
}

int i2cRead(I2C * i2c)
{
	int status;
	do {
		status = read(I2CFd, i2c->gpsBuffer, sizeof(i2c->gpsBuffer));
	} while (-1 == status);
	i2c->bytes = status;
	return status;
}

char CalculateChecksum(char * gpsString, int * index)
{
	char checkSum = 0x00;

	if (index == NULL) {
		index = malloc(sizeof(int));
	}

        *index = 0;

	while (gpsString[*index] != '*') {
		if (gpsString[*index] != '$'){
			checkSum ^= gpsString[*index];
		}
		(*index)++;
	}
	(*index)++;
	return checkSum;
}

int CompareChecksum(char * gpsString)
{
	char providedChecksum = 0x00;
	char offset = 0x00;
	char checkSum;
	int i;

	checkSum = CalculateChecksum(gpsString, &i);

	offset = (gpsString[i] > '9')?('7'):('0');
	providedChecksum |= (gpsString[i] - offset) << 4;

	i++;

	offset = (gpsString[i] > '9')?('7'):('0');
	providedChecksum |= (gpsString[i] - offset);

	//printf("%X %X\n", checkSum, providedChecksum);

	return (checkSum == providedChecksum)?(0):(-1);
}

int GetIndexOfElement(char * gpsString, int elementIndex)
{
        int characterIndex = 0;
        int commaCount = 0;

        while (commaCount < elementIndex) {
                if (',' == gpsString[characterIndex]) {
                        commaCount++;
                }
                characterIndex++;
        }

        return characterIndex;
}


int GetLatLong(char * gpsString, float * latitude, float * longitude)
{
	float tempFloat;
        int index = GetIndexOfElement(gpsString, 2);

	//printf("index = %d\n", index);
	
        if (gpsString[index] == 'A') {
		index = index + 2;
        } else if (gpsString[index] == 'V') {
		printf("Invalid GPS data\n");
                return -1;
        } else if (gpsString[index] == ',') {
		printf("No GPS Signal/Lock\n");
		return -1;
	}

        *latitude = ((gpsString[index] - '0') * 10) + (gpsString[index+1] - '0');
        index = index + 2;
        tempFloat = ((gpsString[index] - '0') * 10) + (gpsString[index+1] - '0');
        index = index + 3;
        tempFloat += ((gpsString[index++] -'0') * 0.1f);
        tempFloat += ((gpsString[index++] -'0') * 0.01f);
        tempFloat += ((gpsString[index++] -'0') * 0.001f);
        tempFloat += ((gpsString[index++] -'0') * 0.0001f);
        tempFloat += ((gpsString[index++] -'0') * 0.00001f);
        tempFloat += ((gpsString[index++] -'0') * 0.000001f);
        index++;
        
        *latitude += tempFloat/60.0f;
 
	//printf("dir = %c\n", gpsString[index]);
        if (gpsString[index] == 'S') {
                *latitude = -(*latitude);
        }

        index = index + 2;

        *longitude = ((gpsString[index] - '0') * 100) + 
                     ((gpsString[index+1] - '0') * 10) + 
                      (gpsString[index+2] - '0');
        index = index + 3;
        tempFloat = ((gpsString[index] - '0') * 10) + (gpsString[index+1] - '0');
        index = index + 3;
        tempFloat += ((gpsString[index++] -'0') * 0.1f);
        tempFloat += ((gpsString[index++] -'0') * 0.01f);
        tempFloat += ((gpsString[index++] -'0') * 0.001f);
        tempFloat += ((gpsString[index++] -'0') * 0.0001f);
        tempFloat += ((gpsString[index++] -'0') * 0.00001f);
        tempFloat += ((gpsString[index++] -'0') * 0.000001f);
        index++;
        
        *longitude += tempFloat/60.0f;

        if (gpsString[index] == 'W') {
                *longitude = -(*longitude);
        }

	return 0;
}


int AppendChecksum(char * gpsCommand, I2C * i2c)
{
	int index;
	sprintf(i2c->gpsBuffer, "%s%X\r\n", gpsCommand, CalculateChecksum(gpsCommand, &index));
	i2c->bytes = index + 4;
	//printf("%d\n", i2c->bytes);
}

int I2CGPSOpen() 
{
	char filename[32];
	int deviceAddress = 0x10;

	memset(filename, 0, sizeof(filename));

	sprintf(filename, "%s", "/dev/i2c-1");

	I2CFd = open(filename, O_RDWR);

	if (I2CFd <= 0) {
		printf("Error opening I2C device\n");
		return -1;
	}

	if (ioctl(I2CFd, I2C_SLAVE, deviceAddress) < 0) {
		printf("Error setting I2C address\n");
		return -1;
	}

	i2cData = malloc(sizeof(I2C));

	memset(i2cData, 0, sizeof(I2C));

	previousIndex = 0;

	return I2CFd;
}

int IsGNGGA(char * nmeaData)
{
	int index;
	int equal;
	char * GNGGA = "$GNGGA";
	for (index = 0; index < 6; index++) {
		equal = (nmeaData[index] == GNGGA[index]);
		if (!equal) break;
	}

	return equal;
}

int ParseNMEA(char * input)
{
	int index = 0;
	int endOfPacket = 0;

	while (index < 255) {
		nmeaBuffer[previousIndex] = input[index];
		previousIndex++;
		if (input[index] == '*') {
			endOfPacket = 1;
		}

		if (input[index++] == '\n') {
			if (endOfPacket && IsGNGGA(nmeaBuffer)) {
				nmeaBufferSize = previousIndex;	
				previousIndex = 0;
				return 0; // no magic numbers please
			 }	
			previousIndex = 0;
			endOfPacket = 0;
		}
	}

	// data at the cutoff is important
	if (index == 255 && IsGNGGA(nmeaBuffer) && !endOfPacket) {
		//previousIndex = previousIndex - 1; // account for CR and LF 
	} else {
		// discard it entirley, should ever hit this.... use an assert
		previousIndex = 0;
	}
	return -1;		
}

int I2CGPSRead(char * message)
{
	int parseStatus;
	float latitude, longitude;
	do
	{
		i2cRead(i2cData);
		parseStatus = ParseNMEA(i2cData->gpsBuffer);

		if (0 == parseStatus &&
		    0 == CompareChecksum(nmeaBuffer) &&
		    0 == GetLatLong(nmeaBuffer, &latitude, &longitude)) {
			break;
		} else {
			usleep(5000);
			//printf("%s\n", nmeaBuffer);
		}
	} while (1);

	//printf("%s\n", nmeaBuffer);
	printf("lat = %f long = %f\n", latitude, longitude);
	memcpy(message, nmeaBuffer, sizeof(nmeaBufferSize));

	return 0;
}

// this will likely be used for sending commands and that's about it
// may restructure this to reflect that...
int I2CGPSWrite(char * command)
{
	AppendChecksum(command, i2cData);
	//printf("%s size = %d\n", i2cData->gpsBuffer, i2cData->bytes);
	i2cWrite(i2cData);
	return 0;
}
