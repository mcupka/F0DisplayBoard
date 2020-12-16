#ifndef DISPLAY_H
#define DISPLAY_H

#include <string.h>
#include "stm32f0xx_hal.h"
#include "gpio.h"

typedef struct DisplayMenu {
	int numOfOptions;
	const char** options;
};

void display_setup();
void display_writecom(uint8_t transmitChar) ;
void display_writedata(uint8_t transmitChar);
void display_print(uint8_t line, const char * text);
int display_present_menu(struct DisplayMenu menu, int (*nav_function)(), int (*sel_function)(), int start_index);
void display_clear();

#endif //DISPLAY_H
