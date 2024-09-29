/*
 * EnergyMonitor.c
 *
 * Created: 31/08/2024 1:18:26 am
 * Author : Y3J5bw
 */ 
#include "common.h"
#include "uart.h"
#include "adc.h"
#include "display.h"
#include "timer0.h"
#include "timer1.h"
#include "timer2.h"
#include "external_interrupts.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

volatile uint8_t capturing = 0;

int main(void)
{
	uart_init(F_CPU/BAUD/16-1); // Iniitalise uart with ubrr number precalculated
	adc_init(); // Initialise ADC
	external_interrupts_init(); // Initialise the external interrupts for zero crossing detection
	display_init(); // Initialise the 7 seg display
	timer0_init(); // Initialise timer0 for refreshing the display at 60Hz
	timer1_init(); // Initialise timer1 for 100 microsecond interrupts for ADC conversion
	timer2_init(); // Initialise timer2 for zero crossing detection
	sei();
	
	// Define the circuit parameters, such as amplification and voltage offsets
	const int32_t VOLTAGE_SCALE = 1 * 22; // 1 for Unity Gain Amplifier and 22 for Voltage Divider
	const int32_t CURRENT_SCALE = 39; // 3.9 but avoiding floats here
	const int32_t OFFSET = 2500; // mV
	const uint16_t ticks_per_period = 500; // Ticks per period on timer1 before compare match A (1 / (2Mhz / 8) / 500Hz)
	
	separate_and_load_characters(0);
	
	/* Replace with your application code */
	while (1)
	{
		// Wait until ADC capturing is complete
		while (capturing) {}
			
		// Process the samples below
		uint32_t phase = time_difference / TOTAL_SAMPLES * 1000 * 2 * M_PI / ticks_per_period; // Times 1000 to avoid decimals
		
		// These two variables will be incremented with the calculated squared values
		int32_t rms_voltage = 0;
		int32_t rms_current = 0;
		
		// Undo the offset and amplification then square the result and store in its rms variable
		for (int i = 0; i < TOTAL_SAMPLES; i++) {
			// Convert from raw ADC value to mV
			int32_t voltage_mV = voltages[i] * 5000UL / 1024;
			int32_t current_mA = currents[i] * 5000UL / 1024;
			/*
			// Undo the offset
			int32_t voltage_diff = voltage_mV - OFFSET;
			int32_t current_diff = current_mA - OFFSET;
			
			// Apply the amplification
			int32_t scaled_voltage = voltage_diff * VOLTAGE_SCALE; 
			int32_t scaled_current = (current_diff * CURRENT_SCALE) / 10; // Change current scale back to 3.9
			
			// Square and increment result in its respective rms variable
			rms_voltage += scaled_voltage * scaled_voltage;
			rms_current += scaled_current * scaled_current;
			*/
			rms_voltage += voltage_mV * voltage_mV;
			rms_current += current_mA * current_mA;
		}
		
		// Need to typecast the calculation as signed 32 bit int because sqrt returns a float
		// Average the rms results then square root it to get the rms value
		rms_voltage = (int32_t)sqrt(rms_voltage / (TOTAL_SAMPLES - 1)); // - 1 because initial i = 0 value is 0000
		rms_current = (int32_t)sqrt(rms_current / TOTAL_SAMPLES);
		
		// Real power calculation with the phase difference, then converted to watts
		int32_t power = (rms_voltage * rms_current) * cos(phase / 1000) / 1000;
		
		separate_and_load_characters(rms_voltage);
		
		char buffer[100];
		snprintf(buffer, sizeof(buffer), "\r\nPhase: %d degrees Time Diff: %lu\r\nRMS Voltage: %lu mV\r\nRMS Current: %lu mA\r\nAverage Power: %lu mW\r\n", (int)(phase * 180 / M_PI / 1000), time_difference / TOTAL_SAMPLES, rms_voltage, rms_current, power);
		uart_transmit_string(buffer);
		
		timer1_init();
		time_difference = 0;

	}
}