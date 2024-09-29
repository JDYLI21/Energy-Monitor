/*
 * adc.h
 *
 * Created: 31/08/2024 1:19:52 am
 *  Author: Y3J5bw
 */ 
#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "common.h"

extern volatile uint8_t sampling;
extern volatile int8_t samples_taken;
extern volatile uint8_t sampling_complete;
extern volatile uint8_t current_channel;
extern volatile uint16_t sample_index;

extern volatile uint16_t voltages[TOTAL_SAMPLES];
extern volatile uint16_t currents[TOTAL_SAMPLES];

void adc_init(void);
void adc_enable(void);
void adc_disable(void);
void adc_reset(void);

#endif