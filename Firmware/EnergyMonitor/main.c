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
	const int32_t VOLTAGE_SCALE = 2782; // 0.849 for Unity Gain Amplifier and 23.63 for Voltage Divider
	const int32_t CURRENT_SCALE = 1212; // 1.212 but avoiding floats here
	const int32_t OFFSET = 2270; // mV
	const int32_t I_SENS = 3065; // 0.30646 ohms 
	
	/* Proteus model
	const int32_t VOLTAGE_SCALE = 2300; // 0.849 for Unity Gain Amplifier and 23.63 for Voltage Divider
	const int32_t CURRENT_SCALE = 3900; // 1.212 but avoiding floats here
	const int32_t OFFSET = 2500; // mV
	const int32_t I_SENS = 2820; // 0.30646 ohms */
	
	// Toggle between displaying RMS voltage (0), peak current (1), or power (2)
	uint8_t disp_param = 0;
	
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
	
		// Somethingkdsrzhgjflkzjruhglidzsruhgrd9gyh9p 843yt9h34
		uint32_t rms_voltage = 0;
		uint32_t peak_current = 0;
		uint32_t power = 0;
		/*
		for (int i = 0; i < sample_index; i++) {
			uart_transmit_count(voltages[i]);
			uart_transmit(',');
			uart_transmit(' ');
			uart_transmit_count(currents[i]);
			uart_transmit(13);
			uart_transmit(10);
		}*/
		
		send_next_character_to_display();
		
		// Undo the offset and amplification then square the result and store in its rms variable
		for (int i = 0; i < sample_index; i++) {
			// Convert from raw ADC value to mV
			int32_t voltage_mV = voltages[i] * 5000UL / 1024;
			int32_t current_mA = currents[i] * 5000UL / 1024;
			
			// Undo the offset
			int32_t voltage_diff = voltage_mV - OFFSET;
			int32_t current_diff = current_mA - OFFSET;
			
			// Apply the amplification
			int32_t scaled_voltage = (voltage_diff * VOLTAGE_SCALE) / 100 / 10; // / 10 is to avoid overflow
			int32_t scaled_current = current_diff * 1000 / CURRENT_SCALE; // Change current scale back to 1.212
			
			// Square and increment result in its respective rms variable
			rms_voltage += scaled_voltage * scaled_voltage;
			if (scaled_current > abs(peak_current)) {
				peak_current = abs(scaled_current);
			}
			uint32_t power_iter = scaled_voltage * scaled_current / I_SENS * 10000;
			power += power_iter;
		}
		
		power = power / 10000 * 10125;
		
		send_next_character_to_display();
		
		// Need to typecast the calculation as signed 32 bit int because sqrt returns a float
		// Average the rms results then square root it to get the rms value
		rms_voltage = (uint32_t)sqrt(rms_voltage / sample_index);
		peak_current = peak_current * 10000 / I_SENS;
		
		// Split rms voltage to integer and decimal
		int rms_voltage_int = rms_voltage / 100;
		int rms_voltage_dec = rms_voltage % 100;
		
		// Split power into first digit for integer and next three for decimal
		int power_int = power / 10000000;
		int power_dec = power % 10000000 / 1000;
		round_num(&power_int, &power_dec);
		
		send_next_character_to_display();
		
		char buffer2[100];
		snprintf(buffer2, sizeof(buffer2), "RMS Voltage: %d.%02dV\r\nPeak Current: 0.%luA\r\nPower: %d.%03dW\r\n", rms_voltage_int, rms_voltage_dec, peak_current, power_int, power_dec);
		uart_transmit_string(buffer2);
		
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
				separate_and_load_characters(round_num_disp(peak_current * 10), disp_param);
				disp_param++;
				break;
			case 2:
				separate_and_load_characters(round_num_disp(power / 10000), disp_param);
				disp_param = 0;
				break;
		}
		
		one_sec_count = 0;
		
		adc_reset();
		external_interrupts_enable();
	}
}