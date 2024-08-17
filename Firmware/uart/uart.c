/*
 * uart.c
 *
 * Created: 17/08/2024 11:51:28 pm
 *  Author: Y3J5bw
 */ 
#include "uart.h"

#include <avr/io.h>
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
void uart_transmit_string(char *str) {
	for (int i = 0; i < strlen(str); i++) {
		uart_transmit(str[i]);
	}
}

// So I don't need so many uart_transmit spam in the main routine
void uart_transmit_newline() {
	// Line Feed then Carriage Return
	// So new line, then point to start of the new line
	uart_transmit(10);
	uart_transmit(13);
}
