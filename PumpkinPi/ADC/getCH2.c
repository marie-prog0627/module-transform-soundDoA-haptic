// for Pumpkin Pi
// (C)Copyright 2016 All rights reserved by Y.Onodera
// http://einstlab.web.fc2.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define SLAVE_ADDRESS	0x68
#define filename	"/dev/i2c-1"


typedef unsigned char       BYTE;	/* 8-bit unsigned  */
typedef unsigned short int  WORD;	/* 16-bit unsigned */


typedef union
{
	WORD Val;
	BYTE v[2];
	short S;
	struct
	{
		BYTE LB;
		BYTE HB;
	} byte;
} WORD_VAL;


typedef union
{
	unsigned char UC;
	struct
	{
		unsigned char G:2;      // 00=1, 01=2, 10=4, 11=8 Gain
		unsigned char S:2;      // 00=12, 01=14, 10=16, 11=18bits
		unsigned char OC:1;     // 0=One-shot, 1=Continuous
		unsigned char C:2;      // 00=channel1, 01=channel2
		unsigned char RDY:1;    // wrinting 1=Initiate, reading 0=Ready
	} bit;
} CONFIG;


int main()
{

	int     i2c;

	CONFIG config;
	WORD_VAL ad;
	char buf[3];

	if( (i2c=open(filename, O_RDWR)) <0 ){
		return 1;
	}

	if(ioctl(i2c, I2C_SLAVE, SLAVE_ADDRESS)<0){
		close(i2c);
		return 1;
	}

	config.bit.RDY=1;
	config.bit.C=1;	// channel2
	config.bit.OC=0;	// One-shot
	config.bit.S=2;	// 16bits
	config.bit.G=0;	// gain=1
	// Initiate Continuous 16bits, 15SPS
	buf[0]=config.UC;
	write(i2c,buf,1);

	// wait 100ms
	usleep(100000);

	read( i2c, buf, 3);
	ad.byte.HB=buf[0];
	ad.byte.LB=buf[1];
	config.UC=buf[2];
	close(i2c);

	printf("%f\n",2.048*ad.S/0x7fff);
	return 0;

}

