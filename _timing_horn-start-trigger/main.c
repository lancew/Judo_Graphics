#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/power.h>
#include "iomacros.h"
#include <avr/sleep.h>


/* Fuse Bits: -U lfuse:w:0x69:m -U hfuse:w:0xff:m */


#define MIC_TTL_INPUT 4,B
#define REED_1000MS_PULSE 3,B

#define UNUSED_PB2 2,B
#define UNUSED_PB1 1,B
#define UNUSED_PB0 0,B


int main(void) {
	out(REED_1000MS_PULSE);
	in(MIC_TTL_INPUT);

	in(UNUSED_PB0);
	in(UNUSED_PB1);
	in(UNUSED_PB2);
	out(UNUSED_PB0);
	out(UNUSED_PB1);
	out(UNUSED_PB2);

	
	for(;;) {
		if(get(MIC_TTL_INPUT)) {
			on(REED_1000MS_PULSE);
			_delay_ms(1000);
		}
		else
			off(REED_1000MS_PULSE);
	}
}

