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

#include "sys/alt_stdio.h"
#include "altera_avalon_pio_regs.h"
//IOWR_ALTERA_AVALON_PIO_DATA(targetAddress, aValue);
//aValue = IOWR_ALTERA_AVALON_PIO_DATA(sourceAddress);

#define load (volatile char *) 0x0003000
#define sent (volatile char *) 0x0003010
#define asoe (volatile char *) 0x0003020
#define mpdataout (volatile char *) 0x0003030
#define chrec (volatile char *) 0x0003040
#define mpdatain (volatile char *) 0x0003050
#define testin (volatile char *) 0x00030c0
#define led (volatile char *) 0x00030a0

int main()
{
	volatile int i;

	alt_putstr("Hello from Nios II!\n");

	while (1){
		*mpdataout=0x4a; //01001010
		*load=1;
		usleep(1000);
		*load=0;
		*asoe=1;
		usleep(1000);;
		while(!(*sent));
		alt_printf("out of while:	sent=%x\n", *sent);
		while(!(*testin));
		alt_putstr("read start\n");
		while(!(*chrec));
		alt_putstr("Almost Finished!\n");
		usleep(1000);
		*asoe=0;
		usleep(100000);
		*led=*mpdatain;
		usleep(1000000);
	}

	return 0;
}