/*
 * external_interrupts.c
 *
 * Created: 2/09/2024 10:43:57 pm
 *  Author: Y3J5bw
 */ 
#include "common.h"
#include "external_interrupts.h"
#include "timer2.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// For storing the zero crossing time / counter when zero crossing is reached
volatile uint32_t time_difference = 0;

// For when voltage's zero crossing has been detected
ISR(INT0_vect) {
	cli();
	timer2_stop();
	// For if timer2 overflows as it is 8 bit and we need to go up to 500
	time_difference += (timer2_overflow_count * 255) + TCNT2; // Add TCNT2 count
	sei();
}

// For when current's zero crossing has been detected
ISR(INT1_vect) {
	timer2_init();
}

// Initialise INT0 and INT1 interrupts for zero crossing
void external_interrupts_init(void) {
	EICRA = (1 << ISC01) | (1 << ISC00) | (1 << ISC11) | (1 << ISC10); // Trigger INT0 and INT1 on rising edge
	EIMSK = (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
}