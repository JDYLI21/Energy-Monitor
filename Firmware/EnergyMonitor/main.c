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
#include "external_interrupts.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

void display_interrupt_routine(void) {
	send_next_character_to_display();	
	set_display = 0;
}

int main(void)
{	
	uart_init(F_CPU/BAUD/16-1); // Iniitalise uart with ubrr number precalculated
	adc_init(); // Initialise ADC
	display_init(); // Initialise the 7 seg display
	timer0_init(); // Initialise timer0 for refreshing the display at 60Hz and ADC autotrigger
	external_interrupts_init(); // Initialise the external interrupts for zero crossing detection
	sei();	
	
	// Define the circuit parameters, such as amplification and voltage offsets
	const int32_t VOLTAGE_SCALE = 1 * 23; // 1 for Unity Gain Amplifier and 23 for Voltage Divider
	const int32_t CURRENT_SCALE = 39; // 3.9 but avoiding floats here
	const int32_t OFFSET = 2500; // mV
	
	// Initially load 00.0V onto the display
	separate_and_load_characters(0);

	/* Replace with your application code */
	while (1)
	{
		// Wait until sampling has finished
		while (!sampling_complete) {
			if (set_display) {
				display_interrupt_routine();
			}
		}
		
		// ------------ DEBUGGING --------------
		char buffer3[9];
		snprintf(buffer3, sizeof(buffer3), "%08u", sample_index);
		uart_transmit_string(buffer3);
		uart_transmit_newline();
		
		// ---------------------------------------
	
		// Somethingkdsrzhgjflkzjruhglidzsruhgrd9gyh9p 843yt9h34
		uint32_t rms_voltage = 0;
		uint32_t peak_current = 0;
		uint32_t power = 0;
		
		// Undo the offset and amplification then square the result and store in its rms variable
		for (int i = 0; i < sample_index; i++) {
			// Convert from raw ADC value to mV
			int32_t voltage_mV = voltages[i] * 5000UL / 1024;
			int32_t current_mA = currents[i] * 5000UL / 1024;
			
			// Undo the offset
			int32_t voltage_diff = voltage_mV - OFFSET;
			int32_t current_diff = current_mA - OFFSET;
			
			// Apply the amplification
			int32_t scaled_voltage = (voltage_diff * VOLTAGE_SCALE) / 10; // / 10 is to avoid overflow
			int32_t scaled_current = current_diff * 10 / CURRENT_SCALE; // Change current scale back to 3.9
			
			// Square and increment result in its respective rms variable
			rms_voltage += scaled_voltage * scaled_voltage; // OVERFLOWS --------------------------------------------
			if (scaled_current > peak_current || scaled_current * -1 > peak_current) {
				peak_current = scaled_current;
			}
			uint32_t power_iter = scaled_voltage * scaled_current * 1000 / 282;
			power += power_iter;
		}
		
		// Need to typecast the calculation as signed 32 bit int because sqrt returns a float
		// Average the rms results then square root it to get the rms value
		rms_voltage = (uint32_t)sqrt(rms_voltage / sample_index);
		peak_current = peak_current * 1000 / 282; // 0.282 is sens resistor
		
		for (int i = 0; i < sample_index; i++) {
			uart_transmit_count(voltages[i]);
			uart_transmit(',');
			uart_transmit(' ');
			uart_transmit_count(currents[i]);
			uart_transmit(13);
			uart_transmit(10);
		} 
		
		
		char buffer2[100];
		snprintf(buffer2, sizeof(buffer2), "RMS Voltage: %lu\r\nPeak Current: %lu\r\nPower: %lu \r\n", rms_voltage, peak_current, power);
		uart_transmit_string(buffer2);
		
		while (one_sec_count < 10000) {
			if (set_display) {
				display_interrupt_routine();
			}
		}
		
		//separate_and_load_characters(power);
		
		one_sec_count = 0;
		
		adc_reset();
		external_interrupts_enable();
	}
}