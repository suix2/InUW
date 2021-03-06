/*
 * comsheader.h
 *
 *  Created on: Dec 1, 2015
 *      Author: suix2
 */

#ifndef COMSHEADER_H_
#define COMSHEADER_H_

#include <stdio.h>
#include <unistd.h>
//#include "sys/alt_stdio.h"
#include "altera_avalon_pio_regs.h"

#define FIGHT_THE_LANDLORD
//#define HANGMAN

#define alt_printf printf
#define alt_putstr printf
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

// SRAM addrs
// cards for shuffle base pointer, 0-53
#define CFSP 0
// number of cards in hands base pointer, 54-56
#define CHNP 54
#define BLACK_JOCKER 13
#define RED_JOCKER 14
#define AAAA 11
#define JJJJ 8
#define QQQQ 9
#define KKKK 10
#define BLACK_JOCKER 13
#define RED_JOCKER 14

#define CUTOFF 15

typedef enum{FALSE, TRUE} Boolean;

// global vars
extern char myNum;
// current cards in hand
extern char* cch;
// player control status
// format 00 00 xx xx
// first 2 bits for landlord number
// last 2 bits for current player
extern char PCS;
// score multiplier
extern char multiplier;
extern int deleteCount;
// used for choosing landlord
extern char firstchoice;
extern char secondchoice;
extern char thirdchoice;
// ll
extern char ll;
extern int score;
extern int rand1, rand2, rand3, rand4;
extern Boolean firstrand;

// Functions
int sramread(char addr);
int sramwrite(char addr, char buf);
int sendchar(int buf);
int sendstr(int bufc, char* bufv);
int recvchar(int* buf);
//int recvstr(int bufc, int* bufv);
char addparity(char c);
unsigned int rand(void);
int shuffle(void);
int landlord(void);
int play(void);
Boolean isCurrentPlayer(void);
int nextPlayer(void);
int incPCS(void);
int resetPCS(void);
int waitNextPlayer(void);
int initCurrentPlayer(void);
int deleteSRAM(char addr);
int syncDeleteSRAM(char addr);
int deleteRecv(void);
int swap(char addr1, char addr2);
int incCardNum(void);
int decCardNum(void);
int syncCardNum(void);
int getCardNum(void);
int getCard(int index);
int initSRAM(void);
Boolean checkPatternAndValue(int* pattern, char* ccp, char num, int* keyValue);
int detectPattern(char* ccp, char num,	char* key);
Boolean haveWinner(void);
void initAnalyzer(char* analyzer);
Boolean comp(int num, char* ch, int start);
Boolean seriesComp(char* ch, int num, int sn, int start);
Boolean seriesCompWithOnes(char* ch, int num, int sn, int one);
int sort(char cn, char* ap);
int display();
Boolean checkCards(int num, char* ccp);
int deleteCards(int num, char* ccp);
void computell(void);
int hangman(void);

#endif /* COMSHEADER_H_ */
