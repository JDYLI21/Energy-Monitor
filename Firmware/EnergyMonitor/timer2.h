/*
 * timer2.h
 *
 * Created: 1/09/2024 12:37:17 am
 *  Author: Y3J5bw
 */ 
#ifndef TIMER2_H
#define TIMER2_H

#include <stdint.h>

extern volatile uint16_t timer2_overflow_count;

void timer2_init(void);
void timer2_stop(void);

#endif