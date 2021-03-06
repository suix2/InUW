/*
 * rand.c
 *
 *  Created on: Dec 1, 2015
 *      Author: suix2
 */
#include "comsheader.h"
/**
 * Generates 8-bit random unsigned integer
 * Inputs: NONE
 * Return: unsigned int generated random number
 */
unsigned int rand(){
   static unsigned int z1, z2, z3, z4;
   unsigned int b;
   if (firstrand){
	   z1 = rand1;
	   z2 = rand2;
	   z3 = rand3;
	   z4 = rand4;
	   firstrand=!firstrand;
   }
   b  = ((z1 << 6) ^ z1) >> 13;
   z1 = ((z1 & 4294967294U) << 18) ^ b;
   b  = ((z2 << 2) ^ z2) >> 27;
   z2 = ((z2 & 4294967288U) << 2) ^ b;
   b  = ((z3 << 13) ^ z3) >> 21;
   z3 = ((z3 & 4294967280U) << 7) ^ b;
   b  = ((z4 << 3) ^ z4) >> 12;
   z4 = ((z4 & 4294967168U) << 13) ^ b;
   return (z1 ^ z2 ^ z3 ^ z4);
}
