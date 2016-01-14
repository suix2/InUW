/*
 * - Use ALT versions of stdio routines:
 *
 *           Function                  Description
 *        ===============  =====================================
 *        alt_printf       Only supports %s, %x, and %c ( < 1 Kbyte)
 *        alt_putstr       Smaller overhead than puts with direct drivers
 *                         Note this function doesn't add a newline.
 *        alt_putchar      Smaller overhead than putchar with direct drivers
 *        alt_getchar      Smaller overhead than getchar with direct drivers
 *
 */
#include "comsheader.h"

//#define SMOKE_SEND
//#define SMOKE_RECV
//#define SRAMTEST
//#define PLAYTEST1
//#define PLAYTEST2
//#define PLAYTEST3

// global vars
char myNum;
char* cch;
char PCS;
char multiplier;
int deleteCount=54;
char firstchoice=0;
char secondchoice=0;
char thirdchoice=0;
char ll = 0;
int score;
int rand1, rand2, rand3, rand4;
Boolean firstrand=TRUE;

int main()
{
	char cardsInHand[20];
	myNum=0;
	PCS=1;
	cch=cardsInHand;
	multiplier=0;
	int i;

	alt_putstr("Fight The Landlord!\n");
	alt_putstr("Please input your number\n");
	alt_putstr("It should be 1, 2, or 3 and unique\n");
	// do not deal with repeat number so far

/******************************/
	// smoke test
#ifdef SMOKE_RECV
	int buf;
	printf("recver\n");
//	recvstr(64, buf);
	while(1){
		recvchar(&buf);
		printf("%d ", buf);
	}
//	recvchar(buf);
//	alt_printf("%x ", *buf);
#endif
#ifdef SMOKE_SEND
	int j, k;
	printf("sender\n");
	for(k=0; k<128; k++){
		for(j=0; j<128; j++){
			sendchar(j);
			usleep(100);
		}
	}
#endif

#ifdef SRAMTEST
//	sramwrite(CHNP, 1);
//	usleep(100);
//	sramwrite(CHNP+1, 2);
//	usleep(100);
//	sramwrite(CHNP+2, 3);
//	usleep(100);
//	printf("%d", sramread(CHNP));
//	printf("%d", sramread(CHNP+1));
//	printf("%d", sramread(CHNP+2));
	for(i=0; i<128; i++){
		sramwrite(i, 127-i);
	}
	for(i=0; i<128; i++){
		printf("%d ", sramread(i));
	}
#endif


#ifdef PLAYTEST1
	PCS=1;
	myNum=1;
	multiplier=1;
	for(i=0; i<20; i++)
		cch[i]=i%13;
	sort(20, cch);
	sramwrite(CHNP, 20);
	sramwrite(CHNP+1, 17);
	sramwrite(CHNP+2, 17);
	usleep(100000);
	play();
	while(1);
#endif

#ifdef PLAYTEST2
	PCS=1;
	myNum=2;
	multiplier=1;
	for(i=0; i<17; i++)
		cch[i]=i%13;
	sort(17, cch);
	sramwrite(CHNP, 20);
	sramwrite(CHNP+1, 17);
	sramwrite(CHNP+2, 17);
	usleep(100000);
	play();
	while(1);
#endif

#ifdef PLAYTEST3
	PCS=1;
	myNum=3;
	multiplier=1;
	for(i=0; i<17; i++)
		cch[i]=i%13;
	sort(17, cch);
	sramwrite(CHNP, 20);
	sramwrite(CHNP+1, 17);
	sramwrite(CHNP+2, 17);
	usleep(100000);
	play();
	while(1);
#endif
/******************************/

#ifdef FIGHT_THE_LANDLORD
	while(myNum!=1 && myNum!=2 && myNum!=3){
		myNum=(char)getchar()-'0';
		printf("OK, you num is: %d\n", myNum);
	}
	printf("Please Input 4 numbers, separated by space, to generate random number");
	scanf("%d %d %d %d", &rand1, &rand2, &rand3, &rand4);
	printf("OK, plase wait...\n");
	rand();

	shuffle();
	landlord();
	computell();
	play();
	while (1){
	}
#endif

#ifdef HANGMAN
	hangman();
#endif
	return 0;
}

void computell() {
	int card1=sramread(0);
	card1=card1%13+13*(card1==52)+13*(card1==53);
	int card2=sramread(1);
	card2=card2%13+13*(card2==52)+13*(card2==53);
	int card3=sramread(2);
	card3=card3%13+13*(card3==52)+13*(card3==53);
    if(firstchoice>=secondchoice && firstchoice>=thirdchoice){
    	ll = 1;
    	multiplier=firstchoice;
    	sramwrite(CHNP, 20);
    	if(myNum==1){
    		score=2;
    		cch[17]=card1;
    		cch[18]=card2;
    		cch[19]=card3;
        	sort(20, cch);
    	}
    	else{
    		sort(17, cch);
    		score=1;
    	}
    } else if(secondchoice>=firstchoice && secondchoice>=thirdchoice){
		ll = 2;
		multiplier=secondchoice;
    	sramwrite(CHNP+1, 20);
    	if(myNum==2){
    		score=2;
    		cch[17]=card1;
    		cch[18]=card2;
    		cch[19]=card3;
        	sort(20, cch);
    	}
    	else{
    		sort(17, cch);
    		score=1;
    	}
	} else if(thirdchoice>=firstchoice && thirdchoice>=secondchoice){
		ll = 3;
		multiplier=thirdchoice;
    	sramwrite(CHNP+2, 20);
    	if(myNum==3){
    		score=2;
    		cch[17]=card1;
    		cch[18]=card2;
    		cch[19]=card3;
        	sort(20, cch);
    	}
    	else{
    		sort(17, cch);
    		score=1;
    	}
	}
	printf("#%d player is the landlord\n", ll);
	printf("The Three Cards Are: %d, %d, %d", card1, card2, card3);
    display();
}
