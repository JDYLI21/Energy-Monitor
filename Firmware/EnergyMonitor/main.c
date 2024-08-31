/*
 * EnergyMonitor.c
 *
 * Created: 31/08/2024 1:18:26 am
 * Author : Y3J5bw
 */ 
#include "common.h"
#include "uart.h"
#include "adc.h"
#include "timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

// Reads the ADC value for a given channel, converts it to its milivolt value then stores
// it in the channel's array in the given iter position
void get_adc_value(uint8_t channel, uint16_t *channel_array, uint8_t iter) {
	uint16_t raw = adc_read(channel);
	uint16_t count = raw * 5000UL / 1024; // Convert from ADC value to milivolts
	channel_array[iter] = count; // Store it in that channels array for that iteration
}

int main(void)
{
	// Iniitalise uart with ubrr number precalculated
	uart_init(F_CPU/BAUD/16-1);
	
	// Initialise ADC
	adc_init();
	
	// Initialise timer1 for zero crossing detection
	timer1_init();
	
	sei();
	
	// Define the circuit parameters, such as amplification and voltage offsets
	const int32_t VOLTAGE_SCALE = 1 * 22; // 1 for Unity Gain Amplifier and 22 for Voltage Divider
	const int32_t CURRENT_SCALE = 39; // 3.9 but avoiding floats here
	const int32_t OFFSET = 2500; // mV
	const float PERIOD_COUNTS = 4000; // 2Mhz / 500Hz
	
	/* Replace with your application code */
	while (1)
	{
		while (!crossing_ready); // Wait until the phase difference is ready to be used
		
		// Calculate the phase difference in radians
		float phase = delta * 2 * M_PI / PERIOD_COUNTS;
		
		// Initialise the arrays to hold the samples for each channel
		uint16_t voltages[100];
		uint16_t currents[100];
		
		// Get 50 samples from each channel
		for (int i = 0; i < 100; i++) {
			get_adc_value(0, voltages, i);
			get_adc_value(1, currents, i);
		}
		
		// These two variables will be incremented with the calculated squared values
		int32_t rms_voltage = 0;
		int32_t rms_current = 0;
		
		// Undo the offset and amplification then square the result and store in its rms variable
		for (int i = 0; i < 100; i++) {
			// Undo the offset
			int32_t voltage_diff = voltages[i] - OFFSET;
			int32_t current_diff = currents[i] - OFFSET;
			
			// Apply the amplification
			int32_t scaled_voltage = voltage_diff * VOLTAGE_SCALE; 
			int32_t scaled_current = (current_diff * CURRENT_SCALE) / 10; // Change current scale back to 3.9

			// Square and increment result in its respective rms variable
			rms_voltage += scaled_voltage * scaled_voltage;
			rms_current += scaled_current * scaled_current;
		}
		
		// Need to typecast the calculation as signed 32 bit int because sqrt returns a float
		// Average the rms results then square root it to get the rms value
		rms_voltage = (int32_t)sqrt(rms_voltage / 100);
		rms_current = (int32_t)sqrt(rms_current / 100);
		
		// Real power calculation with the phase difference, then converted to watts
		int32_t power = (rms_voltage * rms_current) * cos(phase) / 1000;
		
		char buffer[100];
		snprintf(buffer, sizeof(buffer), "\r\nPhase: %d degrees\r\nRMS Voltage: %lu mV\r\nRMS Current: %lu mA\r\nAverage Power: %lu mW\r\n", (int)(phase*180/M_PI), rms_voltage, rms_current, power);
		uart_transmit_string(buffer);
		
		_delay_ms(1000);
	}
}