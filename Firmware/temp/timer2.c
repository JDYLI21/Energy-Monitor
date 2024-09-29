/*
 * timer2.c
 *
 * Created: 1/09/2024 12:37:10 am
 *  Author: Y3J5bw
 */ 
#include "common.h"
#include "timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t timer2_overflow_count = 0;

ISR(TIMER2_OVF_vect) {
	timer2_overflow_count++; // Check for overflow as this is an 8 bit timer
}

void timer2_init(void) {
	TCCR2A = 0; // Normal Mode
	TCCR2B = (1 << CS21); // Prescaler of 8
	TCNT2 = 0; // Start count
}

void timer2_stop(void) {
	TCCR2B &= ~(1 << CS21); // Setting prescaler to 0 stops the timer
}