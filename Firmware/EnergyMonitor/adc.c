/*
 * adc.c
 *
 * Created: 31/08/2024 1:19:47 am
 *  Author: Y3J5bw
 */ 
#include "common.h"
#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// Arrays to hold the adc samples
volatile uint16_t voltages[TOTAL_SAMPLES];
volatile uint16_t currents[TOTAL_SAMPLES];

// Keeping track of channel selected and sample iteration
volatile uint8_t current_channel = 0;
volatile uint16_t sample_index = 0;

ISR(ADC_vect) {
	if (capturing) {
		if (current_channel == 0) {
			voltages[sample_index] = ADC;
		} else {
			currents[sample_index] = ADC;
		}
		
		current_channel ^= 1;
		ADMUX = (ADMUX & 0xF8) | current_channel;
		
		if (current_channel == 0) {
			sample_index++;
		}
		
		if (sample_index >= TOTAL_SAMPLES) {
			capturing = 0;
			TIMSK1 &= ~(1 << OCIE1A);
			sample_index = 0;
		}
	}
}

void adc_init(void) {
	ADMUX |= (1 << REFS0); // Set reference voltage to A_VCC
	ADCSRA |= (1 << ADPS2); // Prescaler of 16 giving us sampling rate of 9.6kHz assuming 2Mhz clock
	ADCSRA = (1 << ADEN) | (1 << ADIE); // Enable ADC and interrupt on conversion
	ADCSRB = 0x00; // Clear ADCSRB register
}