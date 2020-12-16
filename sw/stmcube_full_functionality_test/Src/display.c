#ifndef DISPLAY_C
#define DISPLAY_C

#include "display.h"
#include "main.h"
#include "usart.h"

void display_setup() {

	//Enable GPIO to use with display
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
	GPIOB->MODER |= (GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0);

	//PB5 -> CSB (Active-low)
	//PB6 -> SCL
	//PB7 -> SI
	//PB8 -> RST
	//PB9 -> RS (Active-low)

	//Clear all of them. This also enables RST and CS since those are active-low
	GPIOB->ODR &= ~(GPIO_ODR_5 | GPIO_ODR_6 | GPIO_ODR_7 | GPIO_ODR_8 | GPIO_ODR_9);
	GPIOB->ODR |= GPIO_ODR_8; //Disable reset (active low)

	HAL_Delay(10);


	//initialize, turn on cursor, set address to 0
	uint8_t commands[] = {0x30, 0x30, 0x30, 0x39, 0x14, 0x56, 0x6D, 0x70, 0x0C, 0x06, 0x01, 0x0d, 0x80};

	for (int a = 0; a < (sizeof(commands) / sizeof(commands[0])); a++) {
		display_writecom((commands[a]));
	}

	//Disable CS (active-low)
	GPIOB->ODR |= GPIO_ODR_5;

}

void display_writecom(uint8_t transmitChar) {

	//make sure RS is low
	GPIOB->ODR &= ~(GPIO_ODR_9);

	//Transmit one 8-bit value to display
	for (int a = 0; a < 8; a++) {
		if ((transmitChar & 0x80) == 0x80) {
			//SDA high
			GPIOB->ODR |= GPIO_ODR_7;
		}
		else {
			//SDA low
			GPIOB->ODR &= ~(GPIO_ODR_7);
		}

		transmitChar <<= 1; //Shift bit out

		//Cycle SCL
		GPIOB->ODR |= GPIO_ODR_6;
		HAL_Delay(1);
		GPIOB->ODR &= ~(GPIO_ODR_6);

	}

}

void display_writedata(uint8_t transmitChar) {

	//make sure RS is high
	GPIOB->ODR |= GPIO_ODR_9;

	//Transmit one 8-bit value to display
	for (int a = 0; a < 8; a++) {
		if ((transmitChar & 0x80) == 0x80) {
			//SDA high
			GPIOB->ODR |= GPIO_ODR_7;
		}
		else {
			//SDA low
			GPIOB->ODR &= ~(GPIO_ODR_7);
		}

		transmitChar <<= 1; //Shift bit out

		//Cycle SCL
		GPIOB->ODR |= GPIO_ODR_6;
		HAL_Delay(1);
		GPIOB->ODR &= ~(GPIO_ODR_6);

	}

}

void display_print(uint8_t line, const char * text) {

	//Enable CS (active-low)
	GPIOB->ODR &= ~(GPIO_ODR_5);

	//line 2 starts after 40 characters
	uint8_t set_addr = 0x80 | (40 * line);

	display_writecom(set_addr);

	for (int a = 0; a < strlen(text); a++) {
		display_writedata((uint8_t)text[a]);
	}

	//Disable CS (active-low)
	GPIOB->ODR |= GPIO_ODR_5;

}

int display_present_menu(struct DisplayMenu menu, int (*nav_function)(), int (*sel_function)(), int start_index) {

	int highlighted = start_index;
	int selected = 0;
	int dir_up = 0;
	int navResult = 0;
	int cursorLine = 0;
	int updateDisplay = 0;

	display_clear();
	display_print(0, menu.options[highlighted]);
	display_print(1, menu.options[highlighted + 1]);

	while (selected == 0) {

		//only update display if something has changed
		if (updateDisplay == 1) {

			updateDisplay = 0;
			display_clear();

			//Display the two lines differently depending on if the bottom item it highlighted or the top item is highlighted
			if (dir_up == 0) {
				display_print(0, menu.options[highlighted]);
				if ((highlighted + 1) < menu.numOfOptions) display_print(1, menu.options[(highlighted + 1)]);
				else display_print(1, "------bottom----");
			}
			else {
				display_print(1, menu.options[highlighted]);
				if ((highlighted - 1) >= 0) display_print(0, menu.options[highlighted - 1]);
				else display_print(0, "-------top------");
			}

		}

	    //navigate menu until an option is selected
		navResult = nav_function();


		if (cursorLine == 1) dir_up = 1;
		else if (cursorLine == 0) dir_up = 0;



		if (navResult == 0) updateDisplay = 0;
		else if (highlighted + navResult >= menu.numOfOptions || highlighted < 0) updateDisplay = 0;
		else {
			highlighted += navResult;
			updateDisplay = 1;
		}

		if (cursorLine == 0 && navResult > 0) {
			updateDisplay = 0;
			cursorLine = 1;
		}
		else if (cursorLine == 1 && navResult < 0) {
			updateDisplay = 0;
			cursorLine = 0;
		}

		selected = sel_function();

		char loop_text[35] = "";
		snprintf(loop_text, sizeof(loop_text), "Encoder value: %5lu\r\n", TIM1->CNT);
		HAL_UART_Transmit(&huart2, (unsigned char *)loop_text, sizeof(loop_text), HAL_MAX_DELAY);


	}

	//error;
	return highlighted;
}

void display_clear() {
	display_print(0, "                ");
	display_print(1, "                ");
}


#endif //DISPLAY_C
