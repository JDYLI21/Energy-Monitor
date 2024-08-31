/*
 * adc.c
 *
 * Created: 31/08/2024 1:19:47 am
 *  Author: Y3J5bw
 */ 
#include "adc.h"

#include <avr/io.h>

void adc_init(void) {
	ADMUX |= (1 << REFS0); // Set reference voltage to A_VCC
	ADCSRA |= (1 << ADPS2); // Prescaler of 16 giving us sampling rate of 9.6kHz assuming 2Mhz clock
	ADCSRA |= (1 << ADEN); // Enable ADC
	ADCSRB = 0x00;
}

uint16_t adc_read(uint8_t channel) {
	// First part clears the lower 4 bits so when changing channel only the upper 4 bits which are
	// the MUX bits are changed. Second part sets the upper 4 bits
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC); // Start ADC Conversion
	while (ADCSRA & (1 << ADSC)) {} // Wait for the conversion to finish
	return (ADCL | (ADCH << 8)); // Return the ADC value
}