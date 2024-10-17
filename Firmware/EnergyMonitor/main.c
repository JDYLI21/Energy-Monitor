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
#include <stdlib.h>
#include <math.h>

// Rounds the decimal part of the number and sees if it needs to be 
// carried over to the integer part
void round_num(int *num_int, int *num_dec) {
	if (*num_dec % 10 >= 5) {
		*num_dec = (*num_dec / 10) + 1;
		} else {
		*num_dec = *num_dec / 10;
	}
	
	if (*num_dec == 100) {
		*num_dec = 0;
		*num_int += 1;
	}
}

// Rounds integers only
// Works by adding to see if the next digit should be incremented, then
// abusing integer math by dividing then multiplying by 10 to make the last digit 0
uint32_t round_num_disp(uint32_t num) {
	return ((num + 5) / 10 * 10);
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
	const int32_t VOLTAGE_SCALE = 2234; // 0.849 for Unity Gain Amplifier and 23.63 for Voltage Divider
	const int32_t CURRENT_SCALE = 2825; // 0.2825 but avoiding floats here
	const int32_t OFFSET = 2270; // mV
	const int32_t I_SENS = 3065; // 0.30646 ohms
	
	// Toggle between displaying RMS voltage (0), peak current (1), or power (2)
	uint8_t disp_param = 0;
	
	// Moving average for energy
	uint8_t energy_first_run = 0;
	uint8_t energy_iter = 0;
	uint32_t energy[3];
		
	// Somethingkdsrzhgjflkzjruhglidzsruhgrd9gyh9p 843yt9h34
	int32_t rms_voltage = 0;
	int32_t peak_current = 0;
	int32_t power = 0;
	
	int32_t voltage_mV = 0;
	int32_t current_mA = 0;
	
	int32_t voltage_diff = 0;
	int32_t current_diff = 0;
	
	int32_t scaled_voltage = 0;
	int32_t scaled_current = 0;
	
	int32_t power_iter = 0;
	
	char buffer[100];

	// Initially load 00.0V onto the display
	separate_and_load_characters(0, disp_param);

	/* Replace with your application code */
	while (1)
	{
		// Wait until sampling has finished
		while (!sampling_complete) {
			if (set_display) {
				send_next_character_to_display();
				set_display = 0;
			}
		}
		/*
		char temp[100];
		for (int i = 0; i < sample_index; i++) {
			snprintf(temp, 100, "%u, %u\r\n", voltages[i], currents[i]);
			uart_transmit_string(temp);
		} */
		
		// Undo the offset and amplification then square the result and store in its rms variable
		for (int i = 0; i < sample_index; i++) {
			// Convert from raw ADC value to mV
			voltage_mV = voltages[i] * 5000UL / 1024;
			current_mA = currents[i] * 5000UL / 1024;
			
			// Undo the offset
			voltage_diff = voltage_mV - OFFSET;
			current_diff = current_mA - OFFSET;
			
			// Apply the amplification
			scaled_voltage = (voltage_diff * VOLTAGE_SCALE) / 1000; // / 10 is to avoid overflow
			scaled_current = current_diff * 1000 / CURRENT_SCALE; // Change current scale back to 6.578
			
			// Square and increment result in its respective rms variable
			rms_voltage += scaled_voltage * scaled_voltage;

			if (scaled_current > abs(peak_current)) {
				peak_current = abs(scaled_current);
			}
			
			power_iter = scaled_voltage * scaled_current * 10000 / I_SENS;
			power += power_iter;
			send_next_character_to_display();
		}
		
		power = power / 10000 * 10125;
		
		send_next_character_to_display();
		
		// Need to typecast the calculation as signed 32 bit int because sqrt returns a float
		// Average the rms results then square root it to get the rms value
		rms_voltage = (uint32_t)sqrt(rms_voltage / sample_index);
		peak_current = peak_current * 10000 / I_SENS;
		
		if (!energy_first_run) {
			for (int i = 0; i < 3; i++) {
				energy[i] = power;
				energy_first_run = 1;
			}
		}
		
		energy[energy_iter] = power;
		energy_iter++;
		if (energy_iter == 3) {
			energy_iter = 0;
		}
		
		send_next_character_to_display();
		
		// Split rms voltage to integer and decimal
		int rms_voltage_int = rms_voltage / 100;
		int rms_voltage_dec = rms_voltage % 100;
		
		// Split power into first digit for integer and next three for decimal
		int power_int = power / 10000000;
		int power_dec = power % 10000000 / 1000;
		round_num(&power_int, &power_dec);
	
		// Get the average energy calculation and split the same way as power
		uint32_t energy_avg = (energy[0] + energy[1] + energy[2]) / 3;
		int energy_int = energy_avg / 10000000;
		int energy_dec = energy_avg % 10000000 / 1000;
		round_num(&energy_int, &energy_dec);
		
		send_next_character_to_display();
		
		snprintf(buffer, sizeof(buffer), "RMS Voltage: %d.%02dV\r\nPeak Current: 0.%luA\r\nPower: %d.%03dW\r\nEnergy: %d.%03dW/s\r\n", rms_voltage_int, rms_voltage_dec, peak_current, power_int, power_dec, energy_int, energy_dec);
		uart_transmit_string(buffer);
		
		// Wait till the one second since the start of the loop is over
		while (one_sec_count < 10000) {
			if (set_display) {
				send_next_character_to_display();
				set_display = 0;
			}
		}
		
		// Decides which to display next on the 7 seg
		switch (disp_param) {
			case 0:
				separate_and_load_characters(round_num_disp(rms_voltage), disp_param);
				disp_param++;
				break;
			case 1:
				separate_and_load_characters(round_num_disp(peak_current), disp_param);
				disp_param++;
				break;
			case 2:
				separate_and_load_characters(round_num_disp(power / 10000), disp_param);
				disp_param = 0;
				break;
		}
		
		one_sec_count = 0;
		
		send_next_character_to_display();
		
		power = 0;
		peak_current = 0;
		adc_reset();
		external_interrupts_enable();
	}
}