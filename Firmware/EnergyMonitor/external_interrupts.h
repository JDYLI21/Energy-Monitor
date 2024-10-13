/*
 * external_interrupts.h
 *
 * Created: 2/09/2024 10:44:06 pm
 *  Author: Y3J5bw
 */ 
#ifndef EXTERNAL_INTERRUPTS_H
#define EXTERNAL_INTERRUPTS_H

extern volatile uint16_t time_difference;
extern volatile uint32_t period;
extern volatile uint8_t period_count;

void external_interrupts_init(void);
void external_interrupts_enable(void);
void external_interrupts_disable(void);
void external_interrupts_reset(void);

#endif