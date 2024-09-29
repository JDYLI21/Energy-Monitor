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
#include "external_interrupts.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

void display_interrupt_routine(void) {
	send_next_character_to_display();
	
	if (samples_taken == (uint8_t)NUM_PERIODS) {
		sampling_complete = 1;
	}
	
	set_display = 0;
}

int main(void)
{	
	uart_init(F_CPU/BAUD/16-1); // Iniitalise uart with ubrr number precalculated
	adc_init(); // Initialise ADC
	display_init(); // Initialise the 7 seg display
	timer0_init(); // Initialise timer0 for refreshing the display at 60Hz and ADC autotrigger
	timer1_init(); // Initialise timer1 for measuring period and phase of signal
	external_interrupts_init(); // Initialise the external interrupts for zero crossing detection
	sei();	
	/*
	// Define the circuit parameters, such as amplification and voltage offsets
	const int32_t VOLTAGE_SCALE = 1 * 22; // 1 for Unity Gain Amplifier and 22 for Voltage Divider
	const int32_t CURRENT_SCALE = 39; // 3.9 but avoiding floats here
	const int32_t OFFSET = 2500; // mV
	*/
	
	// Debug -------------------------------------------------
	const int32_t VOLTAGE_SCALE = 141;
	const int32_t CURRENT_SCALE = 2;
	const int32_t OFFSET = 2500;
	// ------------------------------------------------------
	
	// Initially load 0.00V onto the display
	separate_and_load_characters(0);
	
	// Debugging
	DDRB |= (1 << DDB5);
	
	/* Replace with your application code */
	while (1)
	{
		// Wait until sampling has finished
		while (!sampling_complete) {
			if (set_display) {
				display_interrupt_routine();
			}
		}
			
		external_interrupts_disable();
		adc_disable();
		
		// ------------ DEBUGGING --------------
		char buffer[9];
		snprintf(buffer, sizeof(buffer), "%08u", time_difference);
		uart_transmit_string(buffer);
		uart_transmit(',');
		uart_transmit(' ');
		
		char buffer1[9];
		snprintf(buffer1, sizeof(buffer1), "%08u", period);
		uart_transmit_string(buffer1);
		uart_transmit_newline();
		
		char buffer3[9];
		snprintf(buffer3, sizeof(buffer3), "%08u", sample_index);
		uart_transmit_string(buffer3);
		uart_transmit_newline();
		
		// ---------------------------------------
		
		uint32_t phase = ((uint32_t)time_difference * 360) / (uint32_t)period;
		
		// These two variables will be incremented with the calculated squared values
		uint32_t rms_voltage = 0;
		uint32_t rms_current = 0;
		
		// Undo the offset and amplification then square the result and store in its rms variable
		for (int i = 0; i < sample_index; i++) {
			// Convert from raw ADC value to mV
			int32_t voltage_mV = voltages[i] * 5000UL / 1024;
			int32_t current_mA = currents[i] * 5000UL / 1024;
			
			// Undo the offset
			int32_t voltage_diff = voltage_mV - OFFSET;
			int32_t current_diff = current_mA - OFFSET;
			
			// Apply the amplification
			int32_t scaled_voltage = voltage_diff * VOLTAGE_SCALE; 
			int32_t scaled_current = (current_diff * CURRENT_SCALE) / 10; // Change current scale back to 3.9
			
			// Square and increment result in its respective rms variable
			rms_voltage += scaled_voltage * scaled_voltage;
			rms_current += scaled_current * scaled_current;
		}
		
		// Need to typecast the calculation as signed 32 bit int because sqrt returns a float
		// Average the rms results then square root it to get the rms value
		rms_voltage = (uint32_t)sqrt(rms_voltage / sample_index);
		rms_current = (uint32_t)sqrt(rms_current / sample_index);
		
		
		for (int i = 0; i < sample_index; i++) {
			uart_transmit_count(voltages[i]);
			uart_transmit(',');
			uart_transmit(' ');
			uart_transmit_count(currents[i]);
			uart_transmit(13);
			uart_transmit(10);
		} 
		
		char buffer2[100];
		snprintf(buffer2, sizeof(buffer2), "RMS Voltage: %lu mV\r\nRMS Current: %lu mA\r\n", rms_voltage, rms_current);
		uart_transmit_string(buffer2);
		
		while (one_sec_count < 10000) {
			if (set_display) {
				display_interrupt_routine();
			}
		}
		
		separate_and_load_characters(rms_voltage);
		
		one_sec_count = 0;
		
		adc_reset();
		external_interrupts_reset();
		adc_enable();
		external_interrupts_enable();
	}
}