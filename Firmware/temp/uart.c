/*
 * uart.c
 *
 * Created: 31/08/2024 1:44:26 am
 *  Author: Y3J5bw
 */ 
#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

// Same init function as I used in labs 3 and 4
void uart_init(uint16_t ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8); // I used unsigned char here but it's not really necessary
	UBRR0L = (unsigned char)ubrr; // More of a just in case i guess
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
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
	}
}

// So I don't need so many uart_transmit spam in the main routine
void uart_transmit_newline(void) {
	// Line Feed then Carriage Return
	// So new line, then point to start of the new line
	uart_transmit(10);
	uart_transmit(13);
}

void uart_transmit_count(uint16_t count) {
	char ascii_count[4];
	ascii_convert(count, ascii_count);
	for (int i = 0; i < 4; i++) {
		uart_transmit(ascii_count[i]);
	}
}

void ascii_convert(uint16_t count, char *buffer) {
	buffer[0] = (count / 1000) + '0';
	buffer[1] = ((count / 100) % 10) + '0';
	buffer[2] = ((count / 10) % 10) + '0';
	buffer[3] = count % 10 + '0';
}