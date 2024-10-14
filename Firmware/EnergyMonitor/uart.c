/*
 * uart.c
 *
 * Created: 31/08/2024 1:44:26 am
 *  Author: Y3J5bw
 */ 
#include "uart.h"
#include "display.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

// Same init function as I used in labs 3 and 4
void uart_init(uint16_t ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8); // I used unsigned char here but it's not really necessary
	UBRR0L = (unsigned char)ubrr; // More of a just in case i guess
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bit character size
}

// Same transmit function as I used in labs 3 and 4
void uart_transmit(uint8_t data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

// For transmitting the string sequence and the digits so I don't need so many for loops
void uart_transmit_string(const char *str) {
	// Unpacks the str char array and steps through it
	while (*str) {
		uart_transmit(*str++);
		send_next_character_to_display();
	}
}