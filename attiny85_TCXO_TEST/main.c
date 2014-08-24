#include <avr/io.h>
#include <util/delay.h>
#include "iomacros.h"

/* Fuse Bits: -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m */


#define KHZ_1 1,B



int main(void) {
	out(KHZ_1);

	for(;;) {
		__builtin_avr_delay_cycles(6398);
		PORTB = 0b00000010;
		__builtin_avr_delay_cycles(6398);
		PORTB = 0;

	}
}

