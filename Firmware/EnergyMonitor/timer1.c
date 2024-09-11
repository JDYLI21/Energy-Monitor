/*
 * timer1.c
 *
 * Created: 31/08/2024 9:30:47 pm
 *  Author: Y3J5bw
 */ 
#include "common.h"
#include "timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect) {
	if (capturing) {
		ADCSRA |= (1 << ADSC);
	}
}

void timer1_init(void) {
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11); // CTC mode and prescaler of 8
	OCR1A = 24; // 2000000 / 8 / 10000 - 1 or 100 microseconds;
	TIMSK1 |= (1 << OCIE1A); // Enable COMPA interrupt
	TCNT1 = 0;
}