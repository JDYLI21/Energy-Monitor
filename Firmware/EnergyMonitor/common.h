/*
 * common.h
 *
 * Created: 31/08/2024 1:19:33 am
 *  Author: Y3J5bw
 */ 
#ifndef COMMON_H
#define COMMON_H

#include <avr/io.h>

static const uint8_t SAMPLES_PER_PERIOD = 20;
static const uint8_t NUM_PERIODS = 5;
#define TOTAL_SAMPLES 200

#ifndef F_CPU
#define F_CPU 2000000UL
#endif

#endif