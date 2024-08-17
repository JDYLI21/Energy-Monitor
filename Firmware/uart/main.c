/*
 * uart.c
 *
 * Created: 17/08/2024 11:50:17 pm
 * Author : Y3J5bw
 */ 

#include "common.h"
#include "uart.h"

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#define RMSVoltage 14.5
#define PeakCurrent 125
#define Power 1.60

/* Separate functions for each of voltage, current, and power conversion */
void voltage_conversion(char *voltage_array) {
	// I use type casting here to extract only the integer part of the number as it starts off as a float
	
	// Extract the integer part of the RMS Voltage
	uint8_t int_part = (uint8_t) RMSVoltage; 
	
	// Extract the decimal part of the RMSVoltage by removing the integer part then multiplying by 10 so it is an integer
	uint8_t dec_part = (uint8_t) ((RMSVoltage - int_part) * 10); 
	
	voltage_array[0] = (int_part / 10) + '0'; // Tens digit
	voltage_array[1] = (int_part % 10) + '0'; // Ones digit
	voltage_array[2] = '.'; // Decimal point
	voltage_array[3] = dec_part + '0'; // Single decimal digit
}

void current_conversion(char *current_array) {
	// Same conversion as we did in lab 3
	current_array[0] = (PeakCurrent / 100) + '0'; // Hundreds digit
	current_array[1] = ((PeakCurrent / 10) % 10) + '0'; // Tens digit
	current_array[2] = (PeakCurrent % 10) + '0'; // Ones Digit
}

// Similar conversion as voltage except one integer part and two decimal parts
void power_conversion(char *power_array) {
	uint8_t int_part = (uint8_t) Power;
	uint8_t dec_part = (uint8_t) ((Power - int_part) * 100); // times 100 because there's two decimal digits
	
	power_array[0] = int_part + '0'; // Ones digit
	power_array[1] = '.'; // Decimal point
	power_array[2] = (dec_part / 10) + '0'; // Tenths Digit
	power_array[3] = (dec_part % 10) + '0'; // Hundredths Digit
}

int main(void)
{
	// Iniitalise uart with ubrr number precalculated
	uart_init(F_CPU/BAUD/16-1);
	
	// Initialise arrays for the strings before the numbers
	char volt_string[] = "RMS Voltage is: ";
	char current_string[] = "Peak Current is: ";
	char power_string[] = "Power is: ";
	
	// Initialise arrays for the numbers
	char voltage_array[4];
	char current_array[3];
	char power_array[4];
	
	// Fill the number arrays
	voltage_conversion(voltage_array);
	current_conversion(current_array);
	power_conversion(power_array);
	
    /* Replace with your application code */
    while (1) 
    {
		// This should be pretty self explanatory
		// Transmit the string, then the numbers, then a new line
		uart_transmit_string(volt_string);
		uart_transmit_string(voltage_array);
		uart_transmit_newline();
		
		uart_transmit_string(current_string);
		uart_transmit_string(current_array);
		uart_transmit_newline();
		
		uart_transmit_string(power_string);
		uart_transmit_string(power_array);
		uart_transmit_newline();
		
		uart_transmit_newline();
		_delay_ms(1000);
    }
}

