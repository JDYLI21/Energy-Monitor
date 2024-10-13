/*
 * adc.c
 *
 * Created: 31/08/2024 1:19:47 am
 *  Author: Y3J5bw
 */ 
#include "common.h"
#include "adc.h"
#include "timer0.h"
#include "timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// Arrays to hold the adc samples
volatile uint16_t voltages[TOTAL_SAMPLES];
volatile uint16_t currents[TOTAL_SAMPLES];

volatile uint16_t temp = 0;
volatile uint16_t temp_arr[200];
volatile uint8_t iter = 0;

// Keeps track of when the ADC is meant to be sampling
volatile uint8_t sampling = 0;
volatile int8_t samples_taken = -1; // Used in Timer0
volatile uint8_t sampling_complete = 0; // Used in Timer0

// Keeping track of channel selected and sample iteration
volatile uint8_t current_channel = 0;
volatile uint16_t sample_index = 0;

ISR(ADC_vect) {
	//temp = TCNT1;
	if (sampling) {
		//temp_arr[iter] = temp;
		//iter++;
		
		if (current_channel == 0) {
			voltages[sample_index] = ADC;
			} else {
			currents[sample_index] = ADC;
		}
		
		current_channel ^= 1;
		ADMUX = (ADMUX & 0xF0) | (current_channel & 0x0F);
		
		if (current_channel == 0) {
			sample_index++;
		}
	}
}

void adc_init(void) {
	ADMUX |= (1 << REFS0); // Set reference voltage to A_VCC
	ADCSRA = (1 << ADPS1) | (1 << ADPS0); // Prescaler of 8 giving us conversion rate of 19.2kHz assuming 2Mhz clock
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE); // Enable ADC, interrupt on conversion and auto trigger
	ADCSRB = (1 << ADTS1) | (1 << ADTS0); // Set to trigger on Timer0 COMPA
}

void adc_enable(void) {
	ADCSRA |= (1 << ADEN);
	sampling_complete = 0;
}

void adc_disable(void) {
	ADCSRA &= ~(1 << ADEN);
}

void adc_reset(void) {
	for (int i = 0; i < TOTAL_SAMPLES; i++) {
		voltages[i] = 0;
		currents[i] = 0;
	}
	
	sampling = 0;
	samples_taken = -1;
	sampling_complete = 0;
	
	current_channel = 0;
	sample_index = 0;
}