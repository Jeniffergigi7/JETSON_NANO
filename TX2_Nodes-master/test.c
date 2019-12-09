#include <stdio.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#define CTRL_1_G 0x10
#define CTRL_6_X 0x20
#define ODR_238  0x80 
#define USEC_238HZ  4202
#define SAMPLE_T 0.0042f

typedef struct i2c {
	char registerAddress;
	char Buffer[6];
	short int bytes;
} I2C;

int i2cWrite(int i2cFd, I2C * i2c)
{
	int status;
	int bytes = i2c->bytes + sizeof(i2c->registerAddress);
	do {
		status = write(i2cFd, i2c, bytes);
	} while (-1 == status);

	printf("wrote %d, expected %d\n", status, bytes);
}	

int i2cRead(int i2cFd, I2C * i2c)
{
	return i2c_smbus_read_i2c_block_data(i2cFd, i2c->registerAddress, i2c->bytes, i2c->Buffer);
}

void SetRegister(int i2cFd, int registerAddress, char value)
{
	i2c_smbus_write_byte_data(i2cFd, registerAddress, value);
}

int main(int argc, char ** argv)
{
	int i2cFd;
	int writeAddress, readAddress;
	char filename[20];
	char buffer[16];
	int status;
	int i;
	short int x;
	short int y;
	short int z;
	float aAvg;
	int valuesStored = 0;
	int averageCalculated = 0;
	int sampling = 0;

	float xG;
	float yG;
	float zG;

	float baseline = 0;
	float sampledValues[100];
	int baselineValues = 0;
	int initialized = 0;
	float standardDeviation = 0;
	float threshold;
	float samplingStart, samplingEnd;

	float angle;

	useconds_t start;
	useconds_t end;

	I2C out;
	I2C in;

	memset(&out, 0, sizeof(out));
	memset(&in, 0, sizeof(in));

	memset(filename, 0, sizeof(filename));

	sprintf(filename, "%s", "/dev/i2c-0");

	i2cFd = open(filename, O_RDWR);

	if (i2cFd > 0) {
		printf("Success!\n");
	} else {
		printf("errno: %d\n", errno);
		goto closeFD;
		return 0;
	}

	writeAddress = 0x6B;

	if (ioctl(i2cFd, I2C_SLAVE, writeAddress) < 0) {
		printf("Error setting I2C address: %d\n", errno);
		goto closeFD;
	}
	
	// was 0x05
	SetRegister(i2cFd, CTRL_6_X, ODR_238);
	SetRegister(i2cFd, CTRL_1_G, ODR_238); // 100 Hz instead of 800
	SetRegister(i2cFd, 0x04, 0x80);
	SetRegister(i2cFd, 0x1E, 0x22);
	//SetRegister(i2cFd, 0x0E, 0x00);
	//SetRegister(i2cFd, 0x0F, 0x10);
	//SetRegister(i2cFd, 0x2B, 0x02);
	//
	
	in.bytes = 1;
	in.registerAddress = 0x0F;
	i2cRead(i2cFd, &in);
	printf("WO_AM_I = %X\n", in.Buffer[0]);
	i2cRead(i2cFd, &in);
	printf("WO_AM_I = %X\n", in.Buffer[0]);

	float temp1;
	float temp2;
	float temp3;
	float time = 0.0f;

	in.bytes = 2;
	in.registerAddress = 0x1C;
	
	while(1) {
		i2cRead(i2cFd, &in);
	
		z = in.Buffer[0];
		z |= in.Buffer[1] << 8;

		zG = z * ((2.5f * 245.0f) / 65536.0f);
		
		if (!sampling && (zG > 5 || zG < -5)) {
			angle = 0.0f;
			sampling = 1;
			angle += (zG * SAMPLE_T);
		} else if (zG > 1 || zG < -1) {
			angle += (zG * SAMPLE_T);
		} else if (sampling) {
			printf("angle turned = %f\n", angle);
			sampling = 0;
		}
		usleep(USEC_238HZ);
	}

closeFD:
	close(i2cFd);

	return 0;
}
