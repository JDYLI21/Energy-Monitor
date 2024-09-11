/*
 * common.h
 *
 * Created: 31/08/2024 1:19:33 am
 *  Author: Y3J5bw
 */ 
#ifndef COMMON_H
#define COMMON_H

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 2000000UL
#endif

extern volatile uint8_t capturing;

#endif