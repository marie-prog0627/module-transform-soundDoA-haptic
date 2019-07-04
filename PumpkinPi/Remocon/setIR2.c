//**********************
// setIR.c for Raspberry Pi
//
// (1)NEC format
// IR carrier=38KHz
// Time unit=0.56ms
// logical 0 = on 1T + off 1T
// logical 1 = on 1T + off 3T
// reader=on 16T + off 8T
// stop=on 1T
// frame=108ms
//
// (2)AEHA format
// IR carrier=33 - 40KHz
// Time unit=0.35 - 0.50ms
// logical 0 = on 1T + off 1T
// logical 1 = on 1T + off 3T
// reader=on 8T + off 4T
// trailer=on 1T + 8ms
//
// (3)SONY format
// IR carrier=40KHz
// Time unit=0.6ms
// logical 0 = off 1T + on 1T
// logical 1 = off 1T + on 2T
// reader=on 4T
// frame=45ms
//
// caution:
// no detecting repeat code, return bits=0;
//
// (C)Copyright 2015 All rights reserved by Y.Onodera
// http://einstlab.web.fc2.com
//**********************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <sched.h>


#define IR_LIMITS  64  // bytes buffer = IR_LIMITS x8 bits
#define AEHA 21
#define NEC 23
#define SONY 25

void outON(char n, char t);
void outOFF(char n, char t);
void setIR();

unsigned char buf[IR_LIMITS];  // bytes buffer
unsigned char mode;     // 1:NEC, 2:AEHA, 3:SONY
unsigned short bits;    // 32768 bits capable


// irin  = GPIO4  = J8_7
// irout = GPIO13 = J8_33
//#define IRIN  RPI_BPLUS_GPIO_J8_07
//#define IROUT RPI_BPLUS_GPIO_J8_33
#define IRIN  7
#define IROUT 23

// out ON with 38KHz
void outON(char n, char t)
{

	unsigned char i,j;

	for(j=0;j<t;j++){
		for(i=0;i<n;i++){
			// 38KHz, 1/3duty
			digitalWrite(IROUT, 1); // LED ON=8.6us
			delayMicroseconds(8);
			digitalWrite(IROUT, 0);	// LED OFF=17.4us
			delayMicroseconds(16);
		}
	}

}

// out OFF without 38KHz
void outOFF(char n, char t)
{

	unsigned char i,j;

	for(j=0;j<t;j++){
		for(i=0;i<n;i++){
			// 38KHz, 1/3duty
			digitalWrite(IROUT, 0); // LED OFF
			delayMicroseconds(8);
			digitalWrite(IROUT, 0); // LED OFF
			delayMicroseconds(16);
		}
	}

}


void repeat()
{
	unsigned char t;

	if(bits!=0)return;

	switch(mode){
		case 1:
			outON(NEC,16);	// ON(16T)
			outOFF(NEC,4);	// OFF(4T)
			t=16+4;
			t=192-t;
			outON(NEC,1);	// ON(1T)
			outOFF(NEC,t);	// frame=108ms=192T
			break;
		case 2:
			outON(AEHA,8);	// ON(8T)
			outOFF(AEHA,8);	// OFF(8T)
			outON(AEHA,1);	// ON(1T)
			outOFF(AEHA,16); // 8ms=16T
			break;

	}

}


void setIR()
{

	unsigned char j,t;
	unsigned short i,n;

	if(bits==0)return;      // no data

	// reader
	switch(mode){
		case 1:
			if(bits!=32)return;
			outON(NEC,16);  // ON(16T)
			outOFF(NEC,8);  // OFF(8T)
			t=16+8;
			break;
		case 2:
			if(bits>IR_LIMITS*8)return;
			outON(AEHA,8);  // ON(8T)
			outOFF(AEHA,4); // OFF(4T)
			t=8+4;
			break;
		case 3:
			if(!(bits==12 || bits==15 || bits==20))return;
			outON(SONY,4);  // ON(4T)
			t=4;
			break;
	}

	// data
	switch(mode){
		case 1:
			for(i=0;i<4;i++){
				for(j=0;j<8;j++){
					if(buf[i] & (0x1<<j)){
						outON(NEC,1);   // ON(1T)
						outOFF(NEC,3);  // OFF(3T)
						t+=4;
					}else{
						outON(NEC,1);   // ON(1T)
						outOFF(NEC,1);  // OFF(1T)
						t+=2;
					}
				}
			}
			break;
		case 2:
			i=0;
			n=0;
			do{
				for(j=0;j<8;j++){
					if(buf[i] & (0x1<<j)){
						outON(AEHA,1);  // ON(1T)
						outOFF(AEHA,3); // OFF(3T)
						t+=4;
					}else{
						outON(AEHA,1);  // ON(1T)
						outOFF(AEHA,1); // OFF(1T)
						t+=2;
					}
					if(++n == bits)break;
				}
				++i;
			}while(n < bits && i<IR_LIMITS);
			break;
		case 3:
			i=0;
			n=0;
			do{
				for(j=0;j<8;j++){
					if(buf[i] & (0x1<<j)){
						outOFF(SONY,1); // OFF(1T)
						outON(SONY,2);  // ON(2T)
						t+=3;
					}else{
						outOFF(SONY,1); // OFF(1T)
						outON(SONY,1);  // ON(1T)
						t+=2;
					}
					if(++n == bits)break;
				}
				++i;
			}while(n < bits && i<IR_LIMITS);
			break;
	}
   
	// stop
	switch(mode){
		case 1:
			t=192-t;
			outON(NEC,1);   // ON(1T)
			outOFF(NEC,t);  // frame=108ms=192T
			break;
		case 2:
			outON(AEHA,1);  // ON(1T)
			outOFF(AEHA,16); // 8ms=16T
			break;
		case 3:
			t=75-t;
			outOFF(SONY,t); // frame=45ms=75T
			break;
	}
   
}


int main(int argc, char *argv[])
{
	char i,n;
	char name[][5]={"none","NEC ","AEHA","SONY"};
	int minPriority,maxPriority;
        struct sched_param prio;

	// check parameter
	if( argc < 2 ) return 1;

	mode=0;
	if(strcmp(argv[1], "NEC")==0)mode=1;
	if(strcmp(argv[1], "AEHA")==0)mode=2;
	if(strcmp(argv[1], "SONY")==0)mode=3;
	if(mode==0)return 1;


 	bits = atoi(argv[2]);
	if(32768<bits) return 1;

	// set data
	n=(bits+7)/8;    // bits to n bytes
	if( argc != (n+3) ) return 1;
	for(i=0;i<n;i++){
	 	buf[i] = atoi(argv[3+i]);
	}


	// to set high priority
	minPriority=sched_get_priority_min(SCHED_FIFO);
	maxPriority=sched_get_priority_max(SCHED_FIFO);
	prio.sched_priority=maxPriority;
	// printf("%d, %d\n",minPriority,maxPriority);
	if(sched_setscheduler(0,SCHED_FIFO,&prio) < 0){
		printf("error try sudo\n");
		return -1;	
	}

	wiringPiSetup();

	pinMode(IROUT,OUTPUT);
	pinMode(IRIN,INPUT);
//	pullUpDnControl(IRIN,PUD_UP);
	digitalWrite(IROUT,0);

	// set parameter to send out
	// mode is 1:NEC, 2:AEHA or 3:SONY
	// bits is number to send out less than IR_LIMITS x8
	// buf[] are bytes buffer

	repeat();

	setIR();
	// if SONY try 3 times
	if(mode==3){
		setIR();
		setIR();
	}
	return 0;

}


