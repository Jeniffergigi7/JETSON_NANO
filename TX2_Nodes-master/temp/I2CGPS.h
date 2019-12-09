#ifndef I2CGPS_H
#define I2CGPS_H

#include <string.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>

int I2CGPSOpen();

int I2CGPSRead(char * message);

int I2CGPSWrite(char * command);

#endif
