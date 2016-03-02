/*
 * playControl.c
 *
 *  Created on: Dec 2, 2015
 *      Author: suix2
 */

#include "comsheader.h"

/**
 * Delete value at addr in SRAM
 * Different cases are implemented
 * CHANGE CARD NUMBER BEFORE CALLING
 * CHANGE CARD NUMBER BEFORE CALLING
 * CHANGE CARD NUMBER BEFORE CALLING
 * Inputs:
 * 		addr: addr in SRAM where value need to be deleted
 * Return: 0 if success
 */
int deleteSRAM(char addr){
	if(addr<CHNP && addr>=CFSP)
		sramwrite(addr, sramread(deleteCount--));
	else
		return -1;
	return 0;
}

/**
 * synchronizes deleteSRAM operation
 * Inputs: NONE
 * Return: 0 if success
 */
int syncDeleteSRAM(char addr){
	sendchar(3|0x40);
	usleep(250);
	sendchar(addr|0x40);
	usleep(250);
	return 0;
}

/**
 * wait for synchronization of deleteSRAM
 * Inputs: NONE
 * Return: 0 if success
 */
int deleteRecv(){
	int addr;
	recvchar(&addr);
	usleep(50);
	deleteSRAM(addr);
	return 0;
}

/**
 * increase card number by 1
 * Inputs: NONE
 * Return: current number of cards
 */
int incCardNum(){
	char addr=CHNP+myNum-1;
	int cardnum=sramread(addr);
	sramwrite(addr, cardnum+1);
	return cardnum;
}

/**
 * decrease card number by 1
 * Inputs: NONE
 * Return: current number of cards
 */
int decCardNum(){
	char addr=CHNP+myNum-1;
	int cardnum=sramread(addr);
	sramwrite(addr, cardnum-1);
	return cardnum;
}

/**
 * synchronizes card number
 * Inputs: NONE
 * Return: 0 if success
 */
int syncCardNum(){
	char cn[3];
	cn[0]=sramread(CHNP)|0x40;
	cn[1]=sramread(CHNP+1)|0x40;
	cn[2]=sramread(CHNP+2)|0x40;
	sendchar(4|0x40);
	usleep(10000);
	sendstr(3, cn);
	return 0;
}

/**
 * wait for synchronization of syncCardNum
 * Inputs: NONE
 * Return: 0 if success
 */
int getCardNum(){
	int cn[3];
	recvchar(cn);
	usleep(80);
	recvchar(cn+1);
	usleep(80);
	recvchar(cn+2);
	usleep(80);
	sramwrite(CHNP, cn[0]);
	sramwrite(CHNP+1, cn[1]);
	sramwrite(CHNP+2, cn[2]);
	return 0;
}

/**
 * sort cards
 * Inputs:
 * 		cn: number of cards
 * 		ap: array pointer
 * Return: 0 if success
 */
int sort(char cn, char* ap){
	int i, j;
	char temp;
	for(i=0; i<cn; i++)
		for(j=i; j<cn; j++)
			if(ap[i]>ap[j]){
				temp=ap[i];
				ap[i]=ap[j];
				ap[j]=temp;
			}
	return 0;
}
