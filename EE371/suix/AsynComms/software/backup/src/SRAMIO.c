/*
 * SRAMIO.c
 *
 *  Created on: Dec 1, 2015
 *      Author: suix2
 */

#include "comsheader.h"


/**
 * Read the value at given address in SRAM
 * Inputs:
 * 		addr: address in SRAM
 * Return: int value at give address in SRAM
 */
int sramread(char addr){
	IOWR_ALTERA_AVALON_PIO_DATA(noe, 0);
	IOWR_ALTERA_AVALON_PIO_DATA(rnw, 1);
	IOWR_ALTERA_AVALON_PIO_DATA(address, addr);
	return IORD_ALTERA_AVALON_PIO_DATA(data);
}

/**
 * Write given value to given address in SRAM
 * Inputs:
 * 		addr: address in SRAM
 * 		buf: value to be written into SRAM
 * Return: 0 if success
 */
int sramwrite(char addr, char buf){
	IOWR_ALTERA_AVALON_PIO_DATA(rnw, 1);
	IOWR_ALTERA_AVALON_PIO_DATA(noe, 1);
	IOWR_ALTERA_AVALON_PIO_DATA(address, addr);
	IOWR_ALTERA_AVALON_PIO_DATA(data, buf);
	IOWR_ALTERA_AVALON_PIO_DATA(rnw, 0);
	IOWR_ALTERA_AVALON_PIO_DATA(rnw, 1);
	return 0;
}

///**
// * send given char to other boards
// * Inputs:
// * 		buf: char being sent out
// * Return: 0 if success
// */
//int sendchar(char buf){
//
//	//used for test
////	*mpdataout=0x4a; //01001010
////	*mpdataout=0x4b; //01001011
////	*mpdataout=0xff; //11111111
////	*mpdataout=0xaa; //10101010
////	*mpdataout=0x55; //01010101
//
//	*mpdataout = buf*2+addparity(buf);
//
//	*load=1;
//	usleep(100);
//	*load=0;
//	*asoe=1;
//	while(!(*sent));
//	*asoe=0;
//
//	//alt_printf("sent: %x\n", buf);
//	return 0;
//}


/**
 * send given char to other boards
 * Inputs:
 * 		buf: char being sent out
 * Return: 0 if success
 */
int sendchar(int buf){

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

/**
 * send given string to other boards
 * Inputs:
 * 		bufc: number of chars in the string
 * 		bufv: string being sent out
 * Return: 0 if success
 */
int sendstr(int bufc, char* bufv){
	int i;
	for(i=0; i<bufc; i++){
		sendchar(bufv[i]);
		usleep(250);
	}

	return 0;
}

///**
// * receive a char from other board and store it to *buf
// * Inputs:
// * 		buf: points to where the char received stored
// * Return: 0 if success
// *
// */
//int recvchar(char* buf){
//	//check for correctness not implemented so far
//	while(!(*chrec));
//	*buf=(((int)*mpdatain)&0xff)/2;
//	if((*buf)&0x40){
//		*buf &= 0x3f;
//		sendchar(*buf);
//	}
//	return 0;
//}

/**
 * receive a char from other board and store it to *buf
 * Inputs:
 * 		buf: points to where the char received stored
 * Return: 0 if success
 *
 */
int recvchar(int* buf){
	//check for correctness not implemented so far
	while(!IORD_ALTERA_AVALON_PIO_DATA(chrec));
	*buf=(((int)IORD_ALTERA_AVALON_PIO_DATA(mpdatain))&0xff)/2;
#ifdef FIGHT_THE_LANDLORD
	if((*buf)&0x40){
		*buf &= 0x3f;
		sendchar(*buf);
	}
#endif
	return 0;
}

///**
// * receive a string from other board and store it to **buf
// * Inputs:
// * 		bufc: number of chars in the string
// * 		bufv: points to where the string received stored
// * Return: 0 if success
// *
// */
//int recvstr(int bufc, int* bufv){
//	int i;
//	for(i=0; i<bufc; i++){
//		recvchar(bufv+i);
//	}
//	return 0;
//}

/**
 * Swaps 2 values in SRAM
 * Inputs:
 * 		addr1
 * 		addr2
 * Return: 0 if success, 1 if repeat
 */
int swap(char addr1, char addr2){
	if(addr1==addr2) return 1;
	char temp=sramread(addr1);
	sramwrite(addr1, sramread(addr2));
	sramwrite(addr2, temp);
	return 0;
}

/**
 * data = data*2+addparity(data);
 * Adds parity bit for the given char
 * Inputs:
 * 		c: char where parity code need to be added
 * Return: char with parity code added
 */
char addparity(char c) {
	char p = 0;
	int i;
	for(i=0;i<7;i++) {
		p^=c%2;
		c/=2;
	}
	return p;
}
