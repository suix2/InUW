/*
 * play.c
 *
 *  Created on: Dec 1, 2015
 *      Author: suix2
 */

#include "comsheader.h"

/**
 *
 */
int play(){
	int pass=0;
	Boolean firstRound=FALSE;
	// true if first round
	if(ll==myNum)
		firstRound=TRUE;
	// use for "for loop"
	int i;
	// number of cards played
	int number = 0;
	// pattern code
	int pattern=0; //sync
	int temp; //for sync
	// current card played, no need to init
	char ccp[20];
	// key value of card played
	int key=CUTOFF; //sync
	char card[3];
	Boolean check;
	//checkPattern
	printf("Current Player: %d\n", PCS);
	while(!haveWinner()){
		if(isCurrentPlayer()){
			check=TRUE;
			do{
				if(!check)
					printf("Your Inputs Are Illegal, Please Input Again\n");
				check=FALSE;
				//input to ccp, check num here
				display();
				printf("hey player, tell me the number of cards you want to play this round."
						"You can input 0 if you want to give up\n");
				scanf(" %d", &number);
				for(i=0;i<number;i++) {
					if(!i)
						printf("please tell me the card you want to play this round. One by One please. \n");
					else
						printf("please tell me the next card you want to play. \n");
					scanf("%s", card);
					switch(card[0]){
						case 'a':
						case 'A':
							ccp[i]=11;
							break;
						case '2':
							ccp[i]=12;
							break;
						case '3':
							ccp[i]=0;
							break;
						case '4':
							ccp[i]=1;
							break;
						case '5':
							ccp[i]=2;
							break;
						case '6':
							ccp[i]=3;
							break;
						case '7':
							ccp[i]=4;
							break;
						case '8':
							ccp[i]=5;
							break;
						case '9':
							ccp[i]=6;
							break;
						case '1':
							ccp[i]=7;
							break;
						case 'j':
						case 'J':
							ccp[i]=8;
							break;
						case 'q':
						case 'Q':
							ccp[i]=9;
							break;
						case 'k':
						case 'K':
							ccp[i]=10;
							break;
						case 'b':
						case 'B':
							ccp[i]=13;
							break;
						case 'r':
						case 'R':
							ccp[i]=14;
							break;
					}
				}
				if(number && checkCards(number, ccp))
					check=checkPatternAndValue(&pattern, ccp, number, &key);
				printf("check: %d\nkey: %d\n", pattern, key);
			}while((number && !check) || (!number && firstRound));
			if(!number){
				sendchar(CUTOFF | 0x40);
				usleep(10000);
			}
			else{
				if(firstRound)
					firstRound=!firstRound;
				sendchar(key | 0x40);
				usleep(1000);
				sendchar((pattern & 0x3f) | 0x40);
				usleep(1000);
				sendchar((pattern >> 6) | 0x40);
				usleep(1000);
				deleteCards(number, ccp);
				temp=sramread(CHNP+myNum-1);
				sramwrite(CHNP+myNum-1, temp-pattern/10);
				usleep(1000);
				sendchar(sramread(CHNP) | 0x40);
				usleep(1000);
				sendchar(sramread(CHNP+1) | 0x40);
				usleep(1000);
				sendchar(sramread(CHNP+2) | 0x40);
				usleep(1000);
			}
		}
		else{
			recvchar(&temp);
			usleep(80);
			if(temp!=CUTOFF){
				key=temp;
				recvchar(&pattern); //last 6 bits
				usleep(80);
				recvchar(&temp); //11-6 bits of pattern
				usleep(80);
				pattern+=temp*64;
				recvchar(&temp);
				usleep(80);
				sramwrite(CHNP, temp);
				recvchar(&temp);
				usleep(80);
				sramwrite(CHNP+1, temp);
				recvchar(&temp);
				usleep(80);
				sramwrite(CHNP+2, temp);
			}
			else{
				pass++;
				if(pass==2){
					pass=0;
					pattern=0;
					key=0;
				}
			}
		}
		if(pattern==42)
			multiplier*=2;
		temp=sramread(CHNP);
		printf("Pattern Is: %d\nKey Is: %d\nPlayer1 Num Remainds: %d\n", pattern, (key+3)%14+(key==11)+(key==12)+12*(key==13)+12*(key==14), temp);
		temp=sramread(CHNP+1);
		printf("Player2 Num Remainds: %d\n", temp);
		temp=sramread(CHNP+2);
		printf("Player3 Num Remainds: %d\n", temp);
		nextPlayer();
	}
	return 0;
}

Boolean haveWinner(){
	int winner=0;
	if(!sramread(CHNP))
		winner=1;
	else if(!sramread(CHNP+1))
		winner=2;
	else if(!sramread(CHNP+2))
		winner=3;
	if(!winner)
		return FALSE;
	else{
		if(winner==ll && winner==myNum)
			printf("You Win!\nYour Score Is: %d\n", score+score*multiplier);
		else if(winner==ll && winner!=myNum)
			printf("You Lose...\nYour Score Is: %d\n", score-score*multiplier);
		else if(winner!=ll && myNum!=ll)
			printf("You Win!\nYour Score Is: %d\n", score+score*multiplier);
		else
			printf("You Lose...\nYour Score Is: %d\n", score-score*multiplier);
		return TRUE;
	}
}

Boolean checkCards(int num, char* ccp){
	int ccn=sramread(CHNP+myNum-1);
	int check[20];
	int i;
	if((num==4 && comp(4, ccp, 0)) || (ccp[0]==13 && ccp[1]==14) || (ccp[0]==14 && ccp[1]==13))
		return TRUE;
	for(i=0; i<20; i++)
		check[i]=0;
	for(i=0; i<ccn; i++)
		check[(int)cch[i]]++;
	for(i=0; i<num; i++)
		check[(int)ccp[i]]--;
	for(i=0; i<20; i++)
		if(check[i]<0)
			return FALSE;
	return TRUE;
}

// Change Card Number After This Function
int deleteCards(int num, char* ccp){
	int ccn=sramread(CHNP+myNum-1);
	int check[20];
	int i, j, k=0;
	for(i=0; i<20; i++)
		check[i]=0;
	for(i=0; i<ccn; i++)
		check[(int)cch[i]]++;
	for(i=0; i<num; i++)
		check[(int)ccp[i]]--;
	for(i=0; i<15; i++)
		for(j=0; j<check[i]; j++)
			cch[k++]=i;
	return 0;
}

// hi xinyu don't forget to add this to header file lol
int display() {
	int j = 0; // use for "for loop"
	char num=sramread(CHNP+myNum-1); // How many cards the user currently have in hands
	printf("Your cards are: ");
	for(j=0;j<num;j++) {
		switch(cch[j]){
			case 0:
				printf("3 ");
				break;
			case 1:
				printf("4 ");
				break;
			case 2:
				printf("5 ");
				break;
			case 3:
				printf("6 ");
				break;
			case 4:
				printf("7 ");
				break;
			case 5:
				printf("8 ");
				break;
			case 6:
				printf("9 ");
				break;
			case 7:
				printf("10 ");
				break;
			case 8:
				printf("J ");
				break;
			case 9:
				printf("Q ");
				break;
			case 10:
				printf("K ");
				break;
			case 11:
				printf("A ");
				break;
			case 12:
				printf("2 ");
				break;
			case 13:
				printf("B "); //black joker
				break;
			case 14:
				printf("R "); //red joker
				break;
		}

	}
	printf("\n");
	return 0;
}
