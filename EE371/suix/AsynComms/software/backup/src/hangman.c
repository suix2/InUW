/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

//#define TEST_1

#include "comsheader.h"

//#define TEST_1

// HW addrs
#define load (volatile char *) 0x00081010
#define sent (volatile char *) 0x00081020
#define asoe (volatile char *) 0x00081030
#define mpdataout (volatile char *) 0x00081040
#define chrec (volatile char *) 0x00081050
#define mpdatain (volatile char *) 0x00081060
#define noe (volatile char *) 0x00081070
#define rnw (volatile char *) 0x00081080
#define address (volatile char *) 0x00081090
//data in/out of SRAM
#define data (volatile char *) 0x000810a0
#define led (volatile char *) 0x000810b0

int sendchar2(char buf);
int recvchar2(char* buf);

int hangman()
{
#ifdef TEST_1
	char buf=0x55;
	int rec, j, k;
	for(k=0; k<100; k++)
	for(j=0; j<128; j++){
		sendchar(4|0x40);
		usleep(100);
		sendchar(j|0x40);
		usleep(100);
		sendchar(1|0x40);
		usleep(100);
		sendchar(1|0x40);
		usleep(100);
//		recvchar(&rec);
//		printf("rec=%x\n", rec);
	}
	while(1);
#endif

	char ch[63];
	Boolean chb[63];
	Boolean hit=FALSE;
	char guess;
	int res;
	int length;
	int time;
	int myNum;
	int i=0, j;
	int cont=1;
	printf("Please tell me if you want to question(0) or play(1)\n");
	scanf("%d", &myNum);
	if(myNum==0){
		printf("Please tell me the length of your word\n");
		scanf("%d", &length);
		sendchar(length);
		for(j=0; j<length; j++)
			chb[j]=FALSE;
		printf("Please tell me what is you word\n");
		scanf("%s", ch);
		for(j=0; j<length; j++){
			sendchar(ch[j]);
			usleep(100);
		}
		printf("How many chance do you want to give to the player?\n");
		while(time<length){
			printf("time should be longer or equal to the length of the word");
			scanf("%d", &time);
		}
		sendchar(time);
		recvchar(&res);
		if(!res){
			printf("You Lose!");
		}
		else{
			printf("You Win!");
		}
	}
	else{
		recvchar(&length);
		printf("length is: %d\n", length);
		for(j=0; j<length; j++){
			recvchar2(ch+j);
			usleep(50);
		}
		for(j=0; j<length; j++)
			chb[j]=FALSE;
		printf("The Word Is: ");
		for(j=0; j<length; j++)
			printf("%c ", ch[j]);
		recvchar(&time);
		printf("time is: %d\n", time);
		while(cont>0){
			printf("The Word Now Looks Like: ");
			for(j=0; j<length; j++){
				if(!chb[j])
					printf("_ ");
				else
					printf("%c ", ch[j]);
			}
			printf("\nPlease tell me the next letter you guess\n");
			scanf(" %c", &guess);
			printf("the letter you guessed is: %c\n", guess);
			for(j=0; j<length; j++){
				if(ch[j]==guess && !chb[j]){
					i++;
					chb[j]=!chb[j];
					hit=TRUE;
				}
			}
			if(hit){
				hit=FALSE;
				printf("Hit!\n");
			}
			else{
				time--;
				printf("Nope...Times Remaining: %d\n", time);
			}
			if(i==length){
				printf("You Win!\nThe Word Is: ");
				sendchar(0);
				for(j=0; j<length; j++)
					printf("%c ", ch[j]);
				cont=0;
			}
			else if(time<0){
				printf("You Lose!\nThe Word Is: ");
				sendchar(1);
				for(j=0; j<length; j++)
					printf("%c ", ch[j]);
				cont=0;
			}
		}
	}
	while(1);
	return 0;
}

int sendchar2(char buf){

	//used for test
//	*mpdataout=0x4a; //01001010
//	*mpdataout=0x4b; //01001011
//	*mpdataout=0xff; //11111111
//	*mpdataout=0xaa; //10101010
//	*mpdataout=0x55; //01010101

	IOWR_ALTERA_AVALON_PIO_DATA(mpdataout, buf*2+addparity(buf));
	IOWR_ALTERA_AVALON_PIO_DATA(load, 1);
	usleep(100);
	IOWR_ALTERA_AVALON_PIO_DATA(load, 0);
	IOWR_ALTERA_AVALON_PIO_DATA(asoe, 1);
	while(!IORD_ALTERA_AVALON_PIO_DATA(sent));
	IOWR_ALTERA_AVALON_PIO_DATA(asoe, 0);
	//alt_printf("sent: %x\n", buf);
	return 0;
}

int recvchar2(char* buf){
	//check for correctness not implemented so far
	while(!IORD_ALTERA_AVALON_PIO_DATA(chrec));
	*buf=(((int)IORD_ALTERA_AVALON_PIO_DATA(mpdatain))&0xff)/2;
	return 0;
}
