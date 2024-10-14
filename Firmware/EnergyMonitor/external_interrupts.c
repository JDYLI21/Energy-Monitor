/*
 * external_interrupts.c
 *
 * Created: 2/09/2024 10:43:57 pm
 *  Author: Y3J5bw
 */ 
#include "common.h"
#include "external_interrupts.h"
#include "timer0.h"
#include "adc.h"

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// For when voltage's zero crossing has been detected
ISR(INT0_vect) {
	TCNT0 = 0;
	ADCSRA |= (1 << ADATE);
	EIMSK &= ~(1 << INT0);
}

// Initialise INT0 and INT1 interrupts for zero crossing
void external_interrupts_init(void) {
	EICRA = (1 << ISC01) | (1 << ISC00); // Trigger INT0 on rising edge
	EIMSK |= (1 << INT0); // Enable INT0
}

void external_interrupts_enable(void) {
	EIMSK |= (1 << INT0); // Enable INT0 interrupts
}