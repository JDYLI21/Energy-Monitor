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
volatile uint32_t period = 0;
volatile uint8_t period_count = 0;

// Flag for whether ADC should be sampled (0) or display should be refreshed (1)
volatile uint8_t timer0_flag = 0;

// Flag for the start of every sampling iteration to set TCNT1 to 0
volatile uint8_t first_run_flag = 0;

volatile uint8_t sample_count = 0;

// For when voltage's zero crossing has been detected
ISR(INT0_vect) {
	ADCSRA |= (1 << ADSC); // Start ADC conversion
	if (!first_run_flag) {
		TCNT1 = 0;
		first_run_flag = 1;
	} else {
		// Increment with every passing period
		period += TCNT1 + 22; // Delay from INT0 vect till adding TCNT1
		TCNT1 = 0;
		period_count++;
		
		// Refresh display after 2 samples
		if (timer0_flag) {
			sampling = 0;
			samples_taken += 1;
			set_display = 1; //              THE ADC ISR IS WAY TOO LONG TAKING 115 TICKS
			timer0_flag = 0; //              ADC CONVERSION IS ONLY 104 TICKS
		} else {
			TCNT0 = 0;
			sample_count++;
			sampling = 1;
			set_display = 0;
			if (sample_count == 3) {
				sample_count = 0;
				timer0_flag = 1;
			}
		}
	}
}

// Initialise INT0 and INT1 interrupts for zero crossing
void external_interrupts_init(void) {
	EICRA = (1 << ISC01) | (1 << ISC00); // Trigger INT0 on rising edge
	EIMSK |= (1 << INT0); // Enable INT0
}

void external_interrupts_enable(void) {
	EIMSK |= (1 << INT0); // Enable INT0 interrupts
}

void external_interrupts_disable(void) {
	sampling = 0;
	EIMSK &= ~((1 << INT0)); // Disable INT0 interrupts
}

void external_interrupts_reset(void) {
	period = 0;
	period_count = 0;
	
	timer0_flag = 0;
	first_run_flag = 0;
	TCNT1 = 0;
}