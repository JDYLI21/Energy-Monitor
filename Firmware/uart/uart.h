/*
 * uart.h
 *
 * Created: 17/08/2024 11:51:35 pm
 *  Author: Y3J5bw
 */ 
#ifndef UART_H
#define UART_H

#include <avr/io.h>

#ifndef BAUD
#define BAUD 9600
#endif

void uart_init(uint16_t ubrr);
void uart_transmit(uint8_t data);
void uart_transmit_string(const char *str);
void uart_transmit_newline();

#endif