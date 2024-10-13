/*
 * timer0.c
 *
 * Created: 31/08/2024 1:21:46 am
 *  Author: Y3J5bw
 */
#include "timer0.h"
#include "display.h"
#include "common.h"
#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t set_display = 0;
volatile uint16_t refresh_count = 0; // 333 means 30Hz refresh
volatile uint16_t one_sec_count = 0; // 10,000 means one second has passed

/* Timer for ADC Auto Triggering on COMPA and Display Refresh */

ISR(TIMER0_COMPA_vect) {
	refresh_count++;
	one_sec_count++;
	
	if (refresh_count == 333) {
		set_display = 1;
		refresh_count = 0;
	}
}

void timer0_init(void) {
	TCCR0A |= (1 << WGM01); // CTC mode
	TCCR0B |= (1 << CS00); // No prescaler
	OCR0A = 199; // 100 microseconds / (2^8 / 2MHz) * 2^8 - 1 for 100 microseconds autotrigger
	TIMSK0 |= (1 << OCIE0A); // Enable COMPA interrupt
}