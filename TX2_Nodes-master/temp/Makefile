gpstest : gpstest.o I2CGPS.o
	gcc -o gpstest gpstest.o I2CGPS.o

gpstest.o : gpstest.c I2CGPS.h
	gcc -c -o gpstest.o gpstest.c

I2CGPS.o : I2CGPS.c I2CGPS.h
	gcc -c -o I2CGPS.o I2CGPS.c
