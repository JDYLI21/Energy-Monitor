/*
 * adc.h
 *
 * Created: 31/08/2024 1:19:52 am
 *  Author: Y3J5bw
 */ 
#ifndef ADC_H
#define ADC_H

#include <stdint.h>

// The total number of samples of the samples per period multiplied by the amount of periods
// that we are going to sample over. This will change until we are barely within spec.
#define SAMPLES_PER_PERIOD 20
#define NUM_PERIODS 5
#define TOTAL_SAMPLES (SAMPLES_PER_PERIOD * NUM_PERIODS)

extern volatile uint16_t voltages[TOTAL_SAMPLES];
extern volatile uint16_t currents[TOTAL_SAMPLES];

void adc_init(void);

#endif