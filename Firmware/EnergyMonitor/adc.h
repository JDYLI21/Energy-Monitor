/*
 * adc.h
 *
 * Created: 31/08/2024 1:19:52 am
 *  Author: Y3J5bw
 */ 
#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc_init(void);
uint16_t adc_read(uint8_t channel);

#endif