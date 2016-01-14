/*
 * patternDetection.c
 *
 *  Created on: Dec 3, 2015
 *      Author: suix2
 */

#include "comsheader.h"

/**
 *
 * Inputs:
 * 		*pattern: current pattern, 0 if no pattern
 * 		*ccp: current card played
 * 		num: number of cards played
 * 		*key: key value of cards played currently
 * 		**analyzer: analyzed pattern by detectPattern
 * Return: if play is legal
 */
Boolean checkPatternAndValue(int* pattern, char* ccp, char num,	int* keyValue)
{
	int i;
	int mypat;
	char key;
	sort(num, ccp);

	mypat=detectPattern(ccp, num, &key);

	if(mypat==-1)
		return FALSE;
	//check
	if(*pattern==0 || mypat==42){
		*pattern=mypat;
		*keyValue=key;
		return TRUE;
	}
	else if(*pattern==mypat && key>*keyValue){
		*keyValue=key;
		return TRUE;
	}
	return FALSE;
}

/**
 *
 */
int detectPattern(char* ccp, char num,	char* key)
{
	int i;
	int pat;

	//Analyzation
	if(num>4 && num<13){
		for(i=0; i<num-1; i++)
			if(ccp[i]+1 != ccp[i+1])
				goto Label;
		if(ccp[num-1]>11)
			return -1;
		pat=num*10;
		*key=ccp[num-1];
		return pat;
	}

	Label:
	switch(num){
	case 1:
		pat=11;
		*key=ccp[0];
		break;
	case 2:
		if(comp(2, ccp, 0)){
			pat=21;
			*key=ccp[0];
		}
		else if((ccp[0]==BLACK_JOCKER && ccp[1]==RED_JOCKER)||
				(ccp[0]==RED_JOCKER && ccp[1]==BLACK_JOCKER))
		{
			pat=42;
			*key=RED_JOCKER;
		}
		else
			pat=-1;
		break;
	case 3:
		// 3
		if(comp(3, ccp, 0)){
			pat=31;
			*key=ccp[0];
		}
		else
			pat=-1;
		break;
	case 4:
		// 3+1 || 1+3
		if(comp(3, ccp, 0) || comp(3, ccp, 1))
		{
			pat=41;
			*key=ccp[1];
		}
		// 4
		else if(comp(4, ccp, 0)){
			pat=42;
			*key=ccp[0];
		}
		else
			pat=-1;
		break;
	case 5:
		// 3+2 || 2+3
		if((comp(3, ccp, 0) && comp(2, ccp, 3))||
				(comp(2, ccp, 0) && comp(3, ccp, 2)))
		{
			pat=51;
			*key=ccp[2];
		}
		// 4+1 || 1+4
		else if(comp(4, ccp, 0) || comp(4, ccp, 1))
		{
			pat=52;
			*key=ccp[1];
		}
		else
			pat=-1;
		break;
	case 6:
		// 3x2
		if(seriesComp(ccp, 3, 2, 0)){
			pat=61;
			*key=ccp[3];
		}
		// 4+2 || 2+4
		else if((comp(4, ccp, 0) && comp(2, ccp, 4))||
				(comp(2, ccp, 0) && comp(4, ccp, 2)))
		{
			pat=62;
			*key=ccp[2];
		}
		// 2x3
		else if(seriesComp(ccp, 2, 3, 0))
		{
			pat=63;
			*key=ccp[4];
		}
		// 4+1x2 || 1+4+1 || 1x2+4
		else if(seriesCompWithOnes(ccp, 4, 1, 2)){
			pat=64;
			*key=ccp[2];
		}
		else
			pat=-1;
		break;
	case 7:
		// 3x2+1 || 1+3x2
		if(seriesCompWithOnes(ccp, 3, 2, 1))
		{
			pat=71;
			*key=ccp[4];
		}
		else
			pat=-1;
		break;
	case 8:
		// 4x2
		if(seriesComp(ccp, 4, 2, 0)){
			pat=81;
			*key=ccp[4];
		}
		// 2x4
		else if(seriesComp(ccp, 2, 4, 0))
		{
			pat=82;
			*key=ccp[6];
		}
		// 4+2x2 || 2+4+2 || 2x2+4
		else if((comp(4, ccp, 0) && comp(2, ccp, 4) && comp(2, ccp, 6)) ||
				(comp(2, ccp, 0) && comp(4, ccp, 2) && comp(2, ccp, 6)) ||
				(comp(2, ccp, 0) && comp(2, ccp, 2) && comp(4, ccp, 4)))
		{
			pat=83;
			if(comp(4, ccp, 0))
				*key=ccp[0];
			else
				*key=ccp[4];
		}
		// 3x2+2 || 2+3x2
		else if((seriesComp(ccp, 3, 2, 0) && comp(2, ccp, 6)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 3, 2, 2)))
		{
			pat=84;
			*key=ccp[5];
		}
		// 3x2+1x2 || 1+3x2+1 || 1x2+3x2
		else if(seriesCompWithOnes(ccp, 3, 2, 2))
		{
			pat=85;
			*key=ccp[5];
		}
		else
			pat=-1;
		break;
	case 9:
		// 4x2+1 || 1+4x2
		if(seriesCompWithOnes(ccp, 4, 2, 1))
		{
			pat=91;
			*key=ccp[5];
		}
		// 3x3
		if(seriesComp(ccp, 3, 3, 0))
		{
			pat=92;
			*key=ccp[6];
		}
		else
			pat=-1;
		break;
	case 10:
		// 4x2+2 || 2+4x2
		if((seriesComp(ccp, 4, 2, 0) && comp(2, ccp, 8)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 4, 2, 2)))
		{
			pat=101;
			*key=ccp[6];
		}
		// 4x2+1x2 || 1+4x2+1 || 1x2+4x2
		else if(seriesCompWithOnes(ccp, 4, 2, 2))
		{
			pat=102;
			*key=ccp[6];
		}
		// 3x3+1 || 1+3x3
		else if(seriesCompWithOnes(ccp, 3, 3, 1))
		{
			pat=103;
			*key=ccp[7];
		}
		// 2x5
		else if(seriesComp(ccp, 2, 5, 0))
		{
			pat=104;
			*key=ccp[8];
		}
		// 3x2+2x2 || 2+3x2+2 || 2x2+3x2
		else if((seriesComp(ccp, 3, 2, 0) && comp(2, ccp, 6) && comp(2, ccp, 8)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 3, 2, 2) && comp(2, ccp, 8)) ||
				(comp(2, ccp, 0) && comp(2, ccp, 2) && seriesComp(ccp, 3, 2, 4)))
		{
			pat=105;
			if(comp(3, ccp, 0))
				*key=ccp[3];
			else
				*key=ccp[7];
		}
		else
			pat=-1;
		break;
	case 11:
		// 3x3+2 || 2+3x3
		if((seriesComp(ccp, 3, 3, 0) && comp(2, ccp, 9))||
				(comp(2, ccp, 0) && seriesComp(ccp, 3, 3, 2)))
		{
			pat=111;
			*key=ccp[8];
		}
		// 3x3+1x2 || 1+3x3+1 || 1x2+3x3
		else if(seriesCompWithOnes(ccp, 3, 3, 2))
		{
			pat=112;
			*key=ccp[8];
		}
		else
			pat=-1;
		break;
	case 12:
		// 2x6
		if(seriesComp(ccp, 2, 6, 0)){
			pat=121;
			*key=ccp[10];
		}
		// 3x4
		if(seriesComp(ccp, 3, 4, 0)){
			pat=122;
			*key=ccp[9];
		}
		// 4x3
		if(seriesComp(ccp, 4, 3, 0)){
			pat=123;
			*key=ccp[8];
		}
		// 4x2+2x2 || 2+4x2+2 || 2x2+4x2
		if((seriesComp(ccp, 4, 2, 0) && comp(2, ccp, 8) && comp(2, ccp, 10)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 4, 2, 2) && comp(2, ccp, 10))||
				(comp(2, ccp, 0) && comp(2, ccp, 2) && seriesComp(ccp, 4, 2, 4)))
		{
			pat=124;
			if(comp(4, ccp, 0))
				*key=ccp[4];
			else
				*key=ccp[8];
		}
		// 3x3+1x3
		else if(seriesCompWithOnes(ccp, 3, 3, 3)){
			pat=125;
			if(comp(3, ccp, 0))
				*key=ccp[6];
			else
				*key=ccp[9];

		}
		else
			pat=-1;
		break;
	case 13:
		// 3x4+1
		if(seriesCompWithOnes(ccp, 3, 4, 1)){
			pat=131;
			*key=ccp[10];
		}
		// 4x3+1
		else if(seriesCompWithOnes(ccp, 4, 3, 1)){
			pat=132;
			*key=ccp[9];
		}
		// 3x3+2x2
		else if((seriesComp(ccp, 3, 3, 0) && comp(2, ccp, 9) && comp(2, ccp, 11)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 3, 3, 2) && comp(2, ccp, 11))||
				(comp(2, ccp, 0) && comp(2, ccp, 2) && seriesComp(ccp, 3, 3, 4)))
		{
			pat=133;
			if(comp(3, ccp, 0))
				*key=ccp[6];
			else
				*key=ccp[10];
		}
		else
			pat=-1;
		break;
	case 14:
		// 2x7
		if(seriesComp(ccp, 2, 7, 0)){
			pat=141;
			*key=ccp[12];
		}
		// 3x4+2
		else if((seriesComp(ccp, 3, 4, 0) && comp(2, ccp, 12)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 3, 4, 2)))
		{
			pat=142;
			*key=ccp[11];
		}
		// 3x4+1x2
		else if(seriesCompWithOnes(ccp, 3, 4, 2)){
			pat=143;
			*key=ccp[11];
		}
		// 4x3+2
		else if((seriesComp(ccp, 4, 3, 0) && comp(2, ccp, 12)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 4, 3, 2))){
			pat=144;
			*key=ccp[10];
		}
		// 4x3+1x2
		else if(seriesCompWithOnes(ccp, 4, 3, 2)){
			pat=145;
			*key=ccp[10];
		}
		else
			pat=-1;
		break;
	case 15:
		// 3x5
		if(seriesComp(ccp, 3, 5, 0)){
			pat=151;
			*key=ccp[12];
		}
		// 3x4+1x3
		else if(seriesCompWithOnes(ccp, 3, 4, 3)){
			pat=152;
			if(comp(3, ccp, 0))
				*key=ccp[9];
			else
				*key=ccp[12];
		}
		// 4x3+1x3
		else if(seriesCompWithOnes(ccp, 4, 3, 3)){
			pat=153;
			*key=ccp[11];
		}
		else
			pat=-1;
		break;
	case 16:
		// 4x4
		if(seriesComp(ccp, 4, 4, 0)){
			pat=161;
			*key=ccp[13];
		}
		// 2x8
		else if(seriesComp(ccp, 2, 8, 0)){
			pat=162;
			*key=ccp[14];
		}
		// 3x5+1
		else if(seriesCompWithOnes(ccp, 3, 5, 1)){
			pat=163;
			*key=ccp[13];
		}
		// 4x3+2x2
		else if((seriesComp(ccp, 4, 3, 0) && comp(2, ccp, 12) && comp(2, ccp, 14)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 4, 3, 2) && comp(2, ccp, 14))||
				(comp(2, ccp, 0) && comp(2, ccp, 2) && seriesComp(ccp, 4, 3, 4)))
		{
			pat=164;
			if(comp(4, ccp, 0))
				*key=ccp[8];
			else
				*key=ccp[12];
		}
		// 3x4+1x4
		else if(seriesCompWithOnes(ccp, 3, 4, 4)){
			pat=165;
			if(comp(3, ccp, 0) || comp(3, ccp, 1))
				*key=ccp[10];
			else
				*key=ccp[13];
		}
		else
			pat=-1;
		break;
	case 17:
		// 4x4+1
		if(seriesCompWithOnes(ccp, 4, 4, 1)){
			pat=171;
			*key=ccp[14];
		}
		// 3x5+2
		else if((comp(2, ccp, 0) && seriesComp(ccp, 3, 5, 2)) ||
				(seriesComp(ccp, 3, 5, 0) && comp(2, ccp, 15)))
		{
			pat=172;
			*key=ccp[15];
		}
		// 3x5+1x2
		else if(seriesCompWithOnes(ccp, 3, 5, 2)){
			pat=173;
			*key=ccp[15];
		}
		else
			pat=-1;
		break;
	case 18:
		// 2x9
		if(seriesComp(ccp, 2, 9, 0)){
			pat=181;
			*key=ccp[16];
		}
		// 3x6
		else if(seriesComp(ccp, 3, 6, 0)){
			pat=182;
			*key=ccp[15];
		}
		// 3x5+1x3
		else if(seriesCompWithOnes(ccp, 3, 5, 3)){
			pat=183;
			if(comp(3, ccp, 0))
				*key=ccp[12];
			else
				*key=ccp[14];
		}
		// 3x4+2x3 || 2+3x4+2x2 || 2x2+3x4+2 || 2x3+3x4
		else if((seriesComp(ccp, 3, 4, 0) && comp(2, ccp, 12) &&
				comp(2, ccp, 14) && comp(2, ccp, 16)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 3, 4, 2) &&
				comp(2, ccp, 14) && comp(2, ccp, 16)) ||
				(comp(2, ccp, 0) && comp(2, ccp, 2) &&
				seriesComp(ccp, 3, 4, 4) && comp(2, ccp, 16)) ||
				(comp(2, ccp, 0) && comp(2, ccp, 2) &&
				comp(2, ccp, 4) && seriesComp(ccp, 3, 4, 6)))
		{
			pat=184;
			if(comp(3, ccp, 0) || comp(3, ccp, 2))
				*key=ccp[11];
			else
				*key=ccp[15];
		}
		// 4x4+2
		else if((seriesComp(ccp, 4, 4, 0) && comp(2, ccp, 16)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 4, 4, 2)))
		{
			pat=185;
			*key=ccp[14];
		}
		// 4x4+1x2
		else if(seriesCompWithOnes(ccp, 4, 4, 2)){
			pat=186;
			*key=ccp[14];
		}
		else
			pat=-1;
		break;
	case 19:
		// 3x6+1
		if(seriesCompWithOnes(ccp, 3, 6, 1)){
			pat=191;
			*key=ccp[15];
		}
		// 3x5+1x4
		else if(seriesCompWithOnes(ccp, 3, 5, 4)){
			pat=192;
			if(comp(3, ccp, 0) || comp(3, ccp, 1))
				*key=ccp[13];
			else
				*key=ccp[16];
		}
		// 4x4+1x3
		else if(seriesCompWithOnes(ccp, 4, 4, 3)){
			pat=193;
			*key=ccp[15];
		}
		else
			pat=-1;
		break;
	case 20:
		// 2x6+1x2: seriesCompWithOnes(ccp, 2, 6, 2);
		// 2x6: seriesComp(ccp, 2, 6, 0);
		// 2x10
		if(seriesComp(ccp, 2, 10, 0)){
			pat=201;
			*key=ccp[18];
		}
		// 3x6+2
		else if((seriesComp(ccp, 3, 6, 0) && comp(2, ccp, 18))||
				(comp(2, ccp, 0) && seriesComp(ccp, 3, 6, 2)))
		{
			pat=202;
			*key=ccp[17];
		}
		// 3x6+1x2
		else if(seriesCompWithOnes(ccp, 3, 6, 2)){
			pat=203;
			*key=ccp[17];
		}
		// 3x5+1x5
		else if(seriesCompWithOnes(ccp, 3, 5, 5)){
			pat=204;
			if(comp(3, ccp, 0) || comp(3, ccp, 1) || comp(3, ccp, 2))
				*key=ccp[14];
			else
				*key=ccp[17];
		}
		// 4x5
		else if(seriesComp(ccp, 4, 5, 0)){
			pat=205;
			*key=ccp[16];
		}
		// 4x4+2x2 || 2+4x4+2 || 2x2+4x4
		else if((seriesComp(ccp, 4, 4, 0) && comp(2, ccp, 16) && comp(2, ccp, 18)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 4, 4, 2) && comp(2, ccp, 18)) ||
				(comp(2, ccp, 0) && comp(2, ccp, 2) && seriesComp(ccp, 4, 4, 4)))
		{
			pat=206;
			if(comp(4, ccp, 0))
				*key=ccp[12];
			else
				*key=ccp[16];
		}
		// 4x4+1x4
		else if(seriesCompWithOnes(ccp, 4, 4, 1)){
			pat=207;
			if(comp(4, ccp, 0))
				*key=ccp[12];
			else
				*key=ccp[16];
		}
		// 3x4+2x4 || 2+3x4+2x3 || 2x2+3x4+2x2 || 2x3+3x4+2 || 2x4+3x4
		else if((seriesComp(ccp, 3, 4, 0) && comp(2, ccp, 12) && comp(2, ccp, 14) &&
				comp(2, ccp, 16) && comp(2, ccp, 18)) ||
				(comp(2, ccp, 0) && seriesComp(ccp, 3, 4, 2) && comp(2, ccp, 14) &&
				comp(2, ccp, 16) && comp(2, ccp, 18)) ||
				(comp(2, ccp, 0) && comp(2, ccp, 2) && seriesComp(ccp, 3, 4, 4) &&
				comp(2, ccp, 16) && comp(2, ccp, 18)) ||
				(comp(2, ccp, 0) && comp(2, ccp, 2) && comp(2, ccp, 4) &&
				seriesComp(ccp, 3, 4, 6) && comp(2, ccp, 18)) ||
				(comp(2, ccp, 0) && comp(2, ccp, 2) && comp(2, ccp, 4) &&
				comp(2, ccp, 6) && seriesComp(ccp, 3, 4, 8)))
		{
			pat=208;
			if(comp(3, ccp, 0) || comp(3, ccp, 2))
				*key=ccp[11];
			else if(comp(3, ccp, 17))
				*key=ccp[17];
			else
				*key=ccp[15];
		}
		else
			pat=-1;
		break;
	default:
		pat=-1;
		break;
	}

	return pat;
}

Boolean comp(int num, char* ch, int start){
	Boolean res;
	switch(num){
	case 2:
		res=(ch[start]==ch[start+1]);
		break;
	case 3:
		res=(ch[start]==ch[start+1] && ch[start]==ch[start+2]);
		break;
	case 4:
		res=(ch[start]==ch[start+1] && ch[start]==ch[start+2] && ch[start]==ch[start+3]);
		break;
	}
	return res;
}

// 2x6: seriesComp(ccp, 2, 6, 0);
Boolean seriesComp(char* ch, int num, int sn, int start){
	int i;
	Boolean res=TRUE;
	for(i=0; i<sn-1; i++){
		res &= comp(num, ch, start+i*num);
		res &= (ch[num+start+i*num-1]+1==ch[num+start+i*num]);
	}
	res &= comp(num, ch, start+i*num);
	return res;
}

// 2x6+1x2: seriesCompWithOnes(ccp, 2, 6, 2);
Boolean seriesCompWithOnes(char* ch, int num, int sn, int one){
	Boolean res=TRUE;
	int i;
	for (i=0; i<=one; i++)
		res&=seriesComp(ch, num, sn, i);
	return res;
}
