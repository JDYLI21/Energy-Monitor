/*
 * timer1.c
 *
 * Created: 31/08/2024 9:30:47 pm
 *  Author: Y3J5bw
 */ 
#include "common.h"
#include "timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

/* Timer for measuring period and phase of signal */

void timer1_init(void) {
	TCCR1B |= (1 << CS10); // No prescaler
}