/*
 * timer0.c
 *
 * Created: 31/08/2024 1:21:46 am
 *  Author: Y3J5bw
 */ 
#include "timer0.h"
#include "display.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t int_flag = 0;
volatile uint8_t time_count = 0;

ISR(TIMER0_COMPA_vect) {
	send_next_character_to_display();
}

void timer0_init(void) {
	TCCR0A |= (1 << WGM01); // CTC mode
	TCCR0B = (1 << CS01); // Prescaler of 8
	OCR0A = 14; // (F_CPU / (8 * 60Hz in microseconds)) - 1 for 60Hz refresh
	TIMSK0 |= (1 << OCIE0A); // Enable COMPA interrupt
}