/*
 * adc.c
 *
 * Created: 31/08/2024 1:19:47 am
 *  Author: Y3J5bw
 */ 
#include "common.h"
#include "adc.h"
#include "timer0.h"
#include "external_interrupts.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// Arrays to hold the adc samples
volatile uint16_t voltages[TOTAL_SAMPLES];
volatile uint16_t currents[TOTAL_SAMPLES];

// Keeps track of when the ADC is meant to be sampling
volatile uint8_t samples_taken = 0; // number of samples taken
volatile uint8_t sampling_complete = 0; // flag for when sampling is complete

// Keeping track of channel selected and sample iteration
volatile uint8_t current_channel = 0; // 0 for voltage, 1 for current
volatile uint16_t sample_index = 0; // Sample iteration


// ADC interrupt service routine
ISR(ADC_vect) {
	if (current_channel) {
		currents[sample_index] = ADC; // Store the current sample
	} else {
		voltages[sample_index] = ADC; // Store the voltage sample
	}
	
	sample_index++;

	// If we have taken all the samples for the current channel, switch to the other channel
	if (sample_index > SAMPLES_PER_PERIOD * (samples_taken + 1)) {
		current_channel ^= 1; // Switch channel
		ADMUX = (ADMUX & 0xF0) | (current_channel & 0x0F);
		ADCSRA &= ~(1 << ADATE);
		sample_index = 0; // Reset sample index

		external_interrupts_enable(); // Enable external interrupts
		if (!current_channel) { 
			samples_taken++;
			sample_index = SAMPLES_PER_PERIOD * samples_taken;

			// If we have taken all the samples for all the periods, stop sampling
			if (samples_taken == NUM_PERIODS) {
				sampling_complete = 1;
				samples_taken = 0;
				external_interrupts_disable();
			}
		}
	}
}

// Initialise the ADC
void adc_init(void) {
	ADMUX |= (1 << REFS0); // Set reference voltage to A_VCC
	ADCSRA = (1 << ADPS1) | (1 << ADPS0); // Prescaler of 8 giving us conversion rate of 19.2kHz assuming 2Mhz clock
	ADCSRA = (1 << ADEN) | (1 << ADIE); // Enable ADC and interrupt on conversion
	ADCSRB = (1 << ADTS1) | (1 << ADTS0); // Set to trigger on Timer0 COMPA
}


// Reset the ADC variables
void adc_reset(void) {
	for (int i = 0; i < TOTAL_SAMPLES; i++) {
		voltages[i] = 0;
		currents[i] = 0;
	}
	
	sample_index = 0;
	sampling_complete = 0;
}