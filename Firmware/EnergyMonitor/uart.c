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

#define UART_BUFFER_SIZE 64
volatile char uart_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_buffer_read_pos = 0;
volatile uint8_t uart_buffer_write_pos = 0;

ISR(USART0_UDRE_vect) {
	if (uart_buffer_read_pos != uart_buffer_write_pos) {
		UDR0 = uart_buffer[uart_buffer_read_pos];
		uart_buffer_read_pos = (uart_buffer_read_pos + 1) % UART_BUFFER_SIZE;
	} else {
		UCSR0B &= ~(1 << UDRIE0);
	}
	send_next_character_to_display();
}

// Same init function as I used in labs 3 and 4
void uart_init(uint16_t ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8); // I used unsigned char here but it's not really necessary
	UBRR0L = (unsigned char)ubrr; // More of a just in case i guess
	UCSR0B = (1 << TXEN0) | ( 1 << UDRIE0); // Enable UDRE interrupt
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bit character size
}

void uart_transmit(uint8_t data) {
	uart_buffer[uart_buffer_write_pos] = data;
	send_next_character_to_display();
	uart_buffer_write_pos = (uart_buffer_write_pos + 1) % UART_BUFFER_SIZE;
	send_next_character_to_display();
	
	UCSR0B |= (1 << UDRIE0);
}

// For transmitting the string sequence and the digits so I don't need so many for loops
void uart_transmit_string(const char *str) {
	// Unpacks the str char array and steps through it
	while (*str) {
		uart_transmit(*str++);
	}
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