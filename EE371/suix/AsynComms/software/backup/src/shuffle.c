/*
 * shuffle.c
 *
 *  Created on: Dec 1, 2015
 *      Author: suix2
 */

#include "comsheader.h"

/**
 * shuffles the cards
 * Inputs: NONE
 * Return: void
 */
int shuffle(){
	int i;
	int card;
	unsigned int addr;
	int opcode;
	initSRAM();
	while(!isCurrentPlayer()){
		recvchar(&opcode);
		usleep(70);
		switch(opcode){
		case 1:
				nextPlayer();
				break;
		case 3:
				deleteRecv();
				break;
		}
	}
	for(i=0; i<17; i++){
		addr=getCard(i);
		// 0-12 normal, 13 if 52, 14 if 53
		card=sramread(addr);
		cch[i]=card%13+13*(card==52)+13*(card==53);
		deleteSRAM(addr);
		syncDeleteSRAM(addr);
	}
	nextPlayer();
	usleep(250);
	incPCS();
	while(PCS!=1){
		recvchar(&opcode);
		usleep(80);
		switch(opcode){
		case 1:
				nextPlayer();
				break;
		case 3:
				deleteRecv();
				break;
		}
	}
	alt_printf("current player: %x\n", PCS);
	sort(17, cch);
	display();
	return 0;
}

/**
 * gets card value, used for shuffle
 * Inputs:
 * 		index: index of cch
 * Return card value
 */
int getCard(int index){
	return rand()%(54-17*(myNum-1)-index);
}

/**
 * initializes SRAM
 * Inputs: NONE
 * Return: 0 if success
 */
int initSRAM(){
	int i;
	for(i=0; i<54; i++){
		sramwrite(i, i);
	}
	sramwrite(54, 17);
	sramwrite(55, 17);
	sramwrite(56, 17);
	return 0;
}
