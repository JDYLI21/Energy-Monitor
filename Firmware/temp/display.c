/*
 * display.c
 *
 * Created: 31/08/2024 7:16:03 pm
 *  Author: Y3J5bw
 */ 
#include "display.h"

#include <avr/io.h>
#include <stdio.h>

//Array containing which segments to turn on to display a number between 0 to 9
//4 characters to be displayed on Ds1 to Ds 4
static volatile uint8_t disp_characters[4]={0x3E,0,0,0};
//The current digit (e.g. the 1's, the 10's) of the 4-digit number we're displaying
static volatile uint8_t disp_position=0;

// Maps the hex values for each number to the 7 seg layout
const uint8_t seg_map[10] = {
	0x3F,
	0x06,
	0x5B,
	0x4F,
	0x66,
	0x6D,
	0x7D,
	0x07,
	0x7F,
	0x6F
};

void display_init(void) {
	DDRC |= (1 << DDC3) | (1 << DDC4) | (1 << DDC5); // Set SH_CP, SH_DS, SH_ST as output
	DDRD |= (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7); // Set Ds1...Ds4 as output
	PORTD |= (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6); // Turn off Ds1...Ds3
	PORTD &= ~(1 << PORTD7); // Turn on Ds4
}

// Change the number to be displayed
void separate_and_load_characters(uint32_t num) {
	num /= 10;
	
	for (int i = 1; i < 4; i++) {
		uint8_t digit = num % 10;
		disp_characters[i] = seg_map[digit];
		
		// Display the decimal point right after the first digit
		disp_characters[3] |= (1 << 7);
		
		// Shift the number to the next digit for the next iteration
		num /= 10;
	}
}

// Get the number from each digit and display it on each 7 seg display
// This is the function for constantly refreshing the display
void send_next_character_to_display(void) {
	uint8_t pattern = disp_characters[disp_position];
	for (int i = 7; i >= 0; i--) {
		if (pattern & (1 << i)) {
			PORTC |= (1 << PORTC4);
		} else {
			PORTC &= ~(1 << PORTC4);
		}
		
		PORTC |= (1 << PORTC3);
		PORTC &= ~(1 << PORTC3);
	}
	
	PORTD |= (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7);
	
	PORTC |= (1 << PORTC5);
	PORTC &= ~(1 << PORTC5);
	
	switch (disp_position) {
		case 0:
			PORTD &= ~(1 << PORTD7);
			break;
		case 1:
			PORTD &= ~(1 << PORTD6);
			break;
		case 2:
			PORTD &= ~(1 << PORTD5);
			break;
		case 3:
			PORTD &= ~(1 << PORTD4);
			break;
	}
	
	disp_position++;
	if (disp_position >= 4) {
		disp_position = 0;
	}
}