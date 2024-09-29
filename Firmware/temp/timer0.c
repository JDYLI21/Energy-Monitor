/*
 * timer0.c
 *
 * Created: 31/08/2024 1:21:46 am
 *  Author: Y3J5bw
 */ 
#include "timer0.h"
#include "display.h"
#include "common.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t time_count = 0;

ISR(TIMER0_COMPA_vect) {
	if (!capturing) {
		send_next_character_to_display();
	}
}

void timer0_init(void) {
	TCCR0A |= (1 << WGM01); // CTC mode
	TCCR0B = (1 << CS01); // Prescaler of 8
	OCR0A = 49; // (F_CPU / (8 * 20Hz in microseconds)) - 1 for 20Hz refresh
	TIMSK0 |= (1 << OCIE0A); // Enable COMPA interrupt
}