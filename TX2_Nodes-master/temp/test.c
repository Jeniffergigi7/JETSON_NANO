#include <stdio.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

typedef struct i2c {
	union {
		struct add {
			char ADDHI;
			char ADDLO;
		} Address;
		short int AddressFull;
	};
	char Buffer[8];
	short int bytes;
} I2C;

int i2cWrite(int i2cFd, I2C * i2c)
{
	int status;
	int bytes = i2c->bytes + sizeof(i2c->Address);
	do {
		status = write(i2cFd, i2c, bytes);
	} while (-1 == status);

	printf("wrote %d, expected %d\n", status, bytes);
}	

int i2cRead(int i2cFd, I2C * i2c)
{
	// set read address
	int status;
	do {
		status = write(i2cFd, i2c, sizeof(i2c->Address));
	} while (-1 == status);

	read(i2cFd, &i2c->Buffer, sizeof(i2c->Buffer));
}

int main(int argc, char ** argv)
{
	int i2cFd;
	int writeAddress, readAddress;
	char filename[20];
	char buffer[16];
	int status;

	I2C out;
	I2C in;

	memset(&out, 0, sizeof(out));
	memset(&in, 0, sizeof(in));

	memset(filename, 0, sizeof(filename));

	sprintf(filename, "%s", "/dev/i2c-1");

	i2cFd = open(filename, O_RDWR);

	if (i2cFd > 0) {
		printf("Success!\n");
	} else {
		printf("errno: %d\n", errno);
		goto closeFD;
		return 0;
	}

	writeAddress = 0x54;

	if (ioctl(i2cFd, I2C_SLAVE, writeAddress) < 0) {
		printf("Error setting I2C address: %d\n", errno);
		goto closeFD;
	}
	

	out.AddressFull = 0x0102; 
	out.Buffer[0] = 0xDE;
	out.Buffer[1] = 0xAD;
	out.Buffer[2] = 0xBE;
	out.Buffer[3] = 0xEF;
	out.bytes = 4;

	i2cWrite(i2cFd, &out);

	in.AddressFull = 0x0102;
	in.bytes = 4;
	i2cRead(i2cFd, &in);

	int i;
	for (i = 0; i < in.bytes; i++) {
		printf("%X", in.Buffer[i]);
	}
	printf("\n");
closeFD:
	close(i2cFd);

	return 0;
}
