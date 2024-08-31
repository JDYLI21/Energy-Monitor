/*
 * timer1.c
 *
 * Created: 31/08/2024 9:30:47 pm
 *  Author: Y3J5bw
 */ 
#include "timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t voltage_crossing = 0;
volatile uint16_t current_crossing = 0;
volatile uint16_t delta = 0;
volatile uint8_t crossing_ready = 0; // Lets main know both crossings has been reached

// INT0 is voltage crossing
ISR(INT0_vect) {
	voltage_crossing = TCNT1;
}

// INT1 is current crossing
ISR(INT1_vect) {
	current_crossing = TCNT1;
	delta = current_crossing - voltage_crossing;
	crossing_ready = 1;
}

void timer1_init(void) {
	EICRA |= (1 << ISC01) | (1 << ISC11); // Set INT0 and INT1 to trigger on rising edge
	EIMSK |= (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
	TCCR1B |= (1 << CS10); // No prescaler
}