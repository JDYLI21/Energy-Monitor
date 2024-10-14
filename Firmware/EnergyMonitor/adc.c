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
volatile uint8_t samples_taken = 0; // sadasdasdasdasdasdasd
volatile uint8_t sampling_complete = 0; // sdssasdasdsadasdasd

// Keeping track of channel selected and sample iteration
volatile uint8_t current_channel = 0; // 0 for voltage, 1 for current
volatile uint16_t sample_index = 0; // Sample iteration

ISR(ADC_vect) {
	if (current_channel) {
		currents[sample_index] = ADC;
	} else {
		voltages[sample_index] = ADC;
	}
	
	sample_index++;
	if (sample_index > SAMPLES_PER_PERIOD * (samples_taken + 1)) {
		current_channel ^= 1;
		ADMUX = (ADMUX & 0xF0) | (current_channel & 0x0F);
		ADCSRA &= ~(1 << ADATE);
		sample_index = 0;
		external_interrupts_enable();
		if (!current_channel) {
			samples_taken++;
			sample_index = SAMPLES_PER_PERIOD * samples_taken;
			if (samples_taken == NUM_PERIODS) {
				sampling_complete = 1;
				samples_taken = 0;
				external_interrupts_disable();
			}
		}
	}
}

void adc_init(void) {
	ADMUX |= (1 << REFS0); // Set reference voltage to A_VCC
	ADCSRA = (1 << ADPS1) | (1 << ADPS0); // Prescaler of 8 giving us conversion rate of 19.2kHz assuming 2Mhz clock
	ADCSRA = (1 << ADEN) | (1 << ADIE); // Enable ADC and interrupt on conversion
	ADCSRB = (1 << ADTS1) | (1 << ADTS0); // Set to trigger on Timer0 COMPA
}

void adc_reset(void) {
	for (int i = 0; i < TOTAL_SAMPLES; i++) {
		voltages[i] = 0;
		currents[i] = 0;
	}
	
	sample_index = 0;
	sampling_complete = 0;
}