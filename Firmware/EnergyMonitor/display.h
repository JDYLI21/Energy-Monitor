/*
 * display.h
 *
 * Created: 31/08/2024 7:16:09 pm
 *  Author: Y3J5bw
 */ 
#include <stdint.h>

#ifndef DISPLAY_H
#define DISPLAY_H

void display_init(void);
void separate_and_load_characters(uint32_t num);
void send_next_character_to_display(void);

#endif