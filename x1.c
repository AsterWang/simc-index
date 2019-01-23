// test Bits dt

#include <stdio.h>
#include "defs.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"

int main(int argc, char **argv)
{
	Bits b = newBits(60);
	printf("t=0: "); showBits(b); printf("\n");
	setBit(b, 5);
	printf("t=1: "); showBits(b); printf("\n");
	setBit(b, 0);
	setBit(b, 50);
	setBit(b, 59);
	printf("t=2: "); showBits(b); printf("\n");
	if (bitIsSet(b,5)) printf("Bit 5 is set\n");
	if (bitIsSet(b,10)) printf("Bit 10 is set\n");
//    setAllBits(b);
	printf("t=3: "); showBits(b); printf("\n");
	unsetBit(b, 20); //set position to be 0
	printf("t=4: "); showBits(b); printf("\n");
	if (bitIsSet(b,20)) printf("Bit 20 is set\n"); // check this position is 1 
	if (bitIsSet(b,40)) printf("Bit 40 is set\n");
	setBit(b,20);
	if (bitIsSet(b, 20)) printf("Bit 20 is set\n");
	setBit(b, 59);
    if (bitIsSet(b, 59)) printf("Bit 59 is set\n");
	return 0;
}
