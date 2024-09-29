/*
 * external_interrupts.c
 *
 * Created: 2/09/2024 10:43:57 pm
 *  Author: Y3J5bw
 */ 
#include "common.h"
#include "external_interrupts.h"
#include "timer0.h"
#include "timer1.h"
#include "adc.h"

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// For storing the zero crossing time / counter when zero crossing is reached
volatile uint16_t time_difference = 0;
volatile uint16_t period = 0;
volatile uint8_t period_count = 0;

// Flag for whether ADC should be sampled (0) or display should be refreshed (1)
volatile uint8_t timer0_flag = 0;

// Flag for the start of every sampling iteration to set TCNT1 to 0
volatile uint8_t first_run_flag = 0;

// For when voltage's zero crossing has been detected
ISR(INT0_vect) {
	// Increment every time the voltage signal passes by
	time_difference += TCNT1; // Add TCNT1 count
}

// For when current's zero crossing has been detected
ISR(INT1_vect) {
	if (!first_run_flag) {
		first_run_flag = 1;
		TCNT1 = 0;
	} else {
		// Increment with every passing period
		period += TCNT1;
		TCNT1 = 0;
		period_count++;
		
		timer0_flag ^= 1;
		if (timer0_flag) {
			sampling = 0;
			samples_taken += 1;
			set_display = 1;
		} else {
			sampling = 1;
			TCNT0 = 0;
			ADCSRA |= (1 << ADSC); // Start ADC conversion
			set_display = 0;
		}
	}	
}

// Initialise INT0 and INT1 interrupts for zero crossing
void external_interrupts_init(void) {
	EICRA = (1 << ISC01) | (1 << ISC00) | (1 << ISC11) | (1 << ISC10); // Trigger INT0 and INT1 on rising edge
	EIMSK = (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
}

void external_interrupts_enable(void) {
	EIMSK = (1 << INT0) | (1 << INT1); // Enable INT0 and INT1 interrupts
}

void external_interrupts_disable(void) {
	sampling = 0;
	EIMSK &= ~((1 << INT0) | (1 << INT1)); // Disable INT0 and INT1 interrupts
}

void external_interrupts_reset(void) {
	time_difference = 0;
	period = 0;
	period_count = 0;
	
	timer0_flag = 0;
	first_run_flag = 0;
}