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
volatile uint16_t disp_refresh = 0;
volatile uint16_t one_sec_count = 0; // 10,000 means one second has passed

/* Timer for ADC Auto Triggering on COMPA and Display Refresh */

ISR(TIMER0_COMPA_vect) {
	one_sec_count++;
	// If the sampling flag is not set, ADC is no longer sampling so display should refresh
	if (!sampling) {
		disp_refresh++;
		if (disp_refresh == 333); //30Hz refresh rate for display
			set_display = 1;
			disp_refresh = 0;
	}
}

void timer0_init(void) {
	TCCR0A |= (1 << WGM01); // CTC mode
	TCCR0B |= (1 << CS00); // No prescaler
	OCR0A = 199; // 100 microseconds / (2^8 / 2MHz) * 2^8 - 1 for 100 microseconds autotrigger
	TIMSK0 |= (1 << OCIE0A); // Enable COMPA interrupt
}