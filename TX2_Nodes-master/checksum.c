#include <stdio.h>
#include <string.h>

char CalculateChecksum(char * gpsString, int * index)
{
	char checkSum = 0x00;

	if (index == NULL) {
		int temp;
		index = &temp;
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

	printf("%X %X\n", checkSum, providedChecksum);

	return (checkSum == providedChecksum)?(0):(-1);
}

int AppendChecksum(char * gpsCommand, char * buffer)
{
	sprintf(buffer, "%s%X", gpsCommand, CalculateChecksum(gpsCommand, NULL));
}

int main(int argc, char ** argv)
{
	char * gpsString = "$GPGSV,4,1,14,28,75,321,44,42,54,137,39,20,53,080,44,17,40,330,44*";

	char buffer[128];

	AppendChecksum(gpsString, buffer);

	printf("Checksums %s\n", (CompareChecksum(buffer) == 0)?("MATCH"):("DONT MATCH"));	

	return 0;
}
