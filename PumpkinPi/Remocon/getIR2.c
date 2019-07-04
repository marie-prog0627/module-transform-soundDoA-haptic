//**********************
// getIR.c for Raspberry Pi
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
// how to use:
// sudo ./getIR2
// sudo ./getIR2 -n
// sudo ./getIR2 -x
//
// caution:
// no detecting repeat code, return bits=0;
//
// (C)Copyright 2015 All rights reserved by Y.Onodera
// http://einstlab.web.fc2.com
//**********************

#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <sched.h>


#define IR_LIMITS  64  // bytes buffer = IR_LIMITS x8 bits
#define AEHA 21
#define NEC 23
#define SONY 25

unsigned char countHigh();
unsigned char countLow();
void getIR2();
void getIR();

unsigned char buf[IR_LIMITS];	// bytes buffer
unsigned char mode;	// 1:NEC, 2:AEHA, 3:SONY
unsigned int bits;	// 32768 bits capable
unsigned char done;
unsigned char l;


// irin  = GPIO4  = 7
// irout = GPIO13 = 23
#define IRIN  7
#define IROUT 23

unsigned char countHigh(){

	unsigned char i=0;

	// comment to avoid a noise
//	while( digitalRead(IRIN)==1 );	// wait

	while( digitalRead(IRIN)==0 ){
		++i;
		delayMicroseconds(24);
		delayMicroseconds(24);
		if(i==0) return 0;	// timeout 13ms
	}
	// NEC:i=19*8=152, i*2*26.5us=8056us
	// AEHA:i=19*4=76, i*2*26.5us=4028us
	// 1T:i=19*1=19

	return i;

}


unsigned char countLow(){

	unsigned char i=0;

	// comment to void a noise
//	while( digitalRead(IRIN)==0 );	// wait

	while( digitalRead(IRIN)==1 ){
		++i;
		delayMicroseconds(24);
		if(i==0) return 0;	// timeout 6.6ms
	}
	// NEC:i=19*8=152, i*26.5us=4028us
	// AEHA:i=19*4=76, i*26.5us=2014us
	// 1T:i=19*1=19
	// 3T:i=19*3=57

	return i;

}


void getIR2(){

	unsigned char i;
	unsigned short j;   // capable 32768 bits = 4096 bytes
	unsigned char k;

	bits=0;
	for(j=0;j<IR_LIMITS;j++){  // buffer bytes LIMITS
		for(i=0;i<8;i++){   // 8 bits
			k = countHigh()*2;
			if(mode==3){
				buf[j]>>=1;
				// Threschold = 35, 23 = 1T, 46 = 2T; for SONY
				buf[j]+=((k>30) ? 0x80: 0);
				++bits;
			}
			k = countLow();
			if(k==0){
				buf[j]>>=(8-i);
				return;
			}
			if(mode!=3){
				buf[j]>>=1;
				// Threschold = 38, 19 = 1T, 57 = 3T; for NEC
				// Threschold = 30, 15 = 1T, 45 = 3T; for AEHA
				buf[j]+=((k>30) ? 0x80: 0);
				++bits;
			}
		}
	}

}


void display(unsigned char s)
{
        int i,n;
        char name[][5]={"none","NEC ","AEHA","SONY"};

        if(s==2)
        {
                printf("%s",name[mode]);    // 1:NEC, 2:AEHA, 3:SONY
                printf("%02X",bits);    // bits

                // no scroll
                n=(bits+7)/8;    // bits to n bytes
                for(i=0;i<n;i++){
                        printf("%02X",buf[i]);  // data
                }

        }else{

                printf("%s",name[mode]);    // 1:NEC, 2:AEHA, 3:SONY
                printf(" %d",bits);    // bits

                // scroll
                n=(bits+7)/8;    // bits to n bytes
                for(i=0;i<n;i++){
                        printf(" %d",buf[i]);  // data
                }
                printf("\n");
        }
}


void getIR(){

	unsigned char i;

	i = countHigh();    // Start
	mode=0;
	bits=0;

//	printf("%d\n",i);
	if(30<i){
		if(i<51){
			mode=3; // SONY, 46
		}else{
			if(100<i){
				mode=1; // NEC, 173
			}else{
				mode=2; // AEHA, 54-77
			}
		}
		i = countLow();
		getIR2();
		display(l);
	}
//	display(l);
	done=1;
//	delay(10);	// to wait this thread until main thread reading done

}


int main(int argc, char *argv[])
{

	int minPriority,maxPriority;
        struct sched_param prio;
	unsigned char i,n;

	// to set high priority
	minPriority=sched_get_priority_min(SCHED_FIFO);
	maxPriority=sched_get_priority_max(SCHED_FIFO);
	prio.sched_priority=maxPriority;
	// printf("%d, %d\n",minPriority,maxPriority);
	if(sched_setscheduler(0,SCHED_FIFO,&prio) < 0){
		printf("error try sudo\n");
		return -1;
	}

	l=0;
	n=1;
	if(1 < argc){
		if(strcmp(argv[1],"-n")==0)n=10;
		if(strcmp(argv[1],"-x")==0)l=2;
	}
	wiringPiSetup();

	pinMode(IROUT,OUTPUT);
	pinMode(IRIN,INPUT);
	pullUpDnControl(IRIN,PUD_UP);
	digitalWrite(IROUT,0);

	// to detect falling edge
	// getting a little noise
	wiringPiISR(IRIN,INT_EDGE_FALLING,(void*)getIR);
//	waitForInterrupt(IRIN,-1);

	for(i=0;i<n;i++){
		done = 0;
		while(done==0){
			delay(5);
		}
	}

}


