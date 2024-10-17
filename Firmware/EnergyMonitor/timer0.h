/*
 * timer0.h
 *
 * Created: 31/08/2024 1:22:16 am
 *  Author: Y3J5bw
 */ 
#include <stdint.h>

#ifndef TIMER0_H
#define TIMER0_H

extern volatile uint8_t set_display;
extern volatile uint16_t one_sec_count;

void timer0_init(void);

#endif