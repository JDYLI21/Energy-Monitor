/*
 * timer1.h
 *
 * Created: 31/08/2024 9:30:52 pm
 *  Author: Y3J5bw
 */ 
#ifndef TIMER1_H
#define TIMER1_H

#include <stdint.h>

extern volatile uint16_t delta;
extern volatile uint8_t crossing_ready;

void timer1_init(void);

#endif