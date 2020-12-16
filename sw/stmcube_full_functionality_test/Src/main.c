/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "display.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define ENCODER_SENSITIVITY 10

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void interactive_test_menu(void);
void led_test(void);
void encoder_test(void);
void button_test(void);
void display_test(void);
int encoder_navigate();
int encoder_select();
void encoder_init();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

typedef struct MenuOption {
	const char * menuText;
	char selectionChar;
  void (*function)();
};


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  display_setup();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  const char test_message[] = "F0 Display Board by Michael Cupka, Hardware Test Build\r\n";

  display_test();

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  HAL_UART_Transmit(&huart2, (unsigned char *)test_message, sizeof(test_message), HAL_MAX_DELAY);

	  interactive_test_menu();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void interactive_test_menu(void) {
	//print initial menu
	char userSelection;
	  struct MenuOption ledTestOption = {
	    .menuText = "LEDs",
	    .selectionChar = '1',
	    .function = &led_test
	  };
	  struct MenuOption encoderTestOption = {
	    .menuText = "Encoder",
	    .selectionChar = '2',
	    .function = &encoder_test
	  };
	  struct MenuOption displayTestOption = {
	    .menuText = "Display",
	    .selectionChar = '3',
	    .function = &display_test
	  };


	//----TESTING DISPLAY MENU STUFF--//
	struct MenuOption menu[] = {ledTestOption, encoderTestOption, displayTestOption};

	char cbuffer[20] = {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1' , '1', '1', '1', '1'};
	char cbuffer2[20] = {'2', '2', '2', '2', '2', '2', '2', '2', '2', '2', '2', '2' , '2', '2', '2', '2'};
	char cbuffer3[20] = {'3', '3', '3', '3', '3', '3', '3', '3', '3', '3', '3', '3' , '3', '3', '3', '3'};
	char * displayMenuOptions[6] = {ledTestOption.menuText, encoderTestOption.menuText, displayTestOption.menuText};
	struct DisplayMenu m = {
			.numOfOptions = 3,
			.options = displayMenuOptions
	};
	encoder_init();

	while (1) {
		int selected_test = display_present_menu(m, &encoder_navigate, &encoder_select, 0);
		menu[selected_test].function();
	}
	//snprintf(cbuffer, 20, "Value: %d       ", display_present_menu(m, &encoder_navigate, &encoder_select, 0));
	//display_print(0, (const char *)cbuffer);
	////////////////////////////////////////////////

	const char newline[] = "\r\n";
	const char whitespace[] = "\.\t\t";

	for (int a = 0; a < (sizeof(menu) / sizeof(menu[0])); a++) {
		//print each menu option
		HAL_UART_Transmit(&huart2, (unsigned char *)&(menu[a].selectionChar), sizeof(menu[a].selectionChar), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (unsigned char *)whitespace, sizeof(whitespace), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (unsigned char *)menu[a].menuText, strlen(menu[a].menuText), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (unsigned char *)newline, sizeof(newline), HAL_MAX_DELAY);
	}

	//get input from the user
	HAL_UART_Receive (&huart2, &userSelection, 1, HAL_MAX_DELAY);

	//go to appropriate test
	for (int a = 0; a < (sizeof(menu) / sizeof(menu[0])); a++) {
		if (menu[a].selectionChar == userSelection) menu[a].function();
	}

	return;
}

void led_test(void) {
	char text[] = "YOU ARE TESTING LEDS\r\n\r\n";

	HAL_UART_Transmit(&huart2, (unsigned char *)text, sizeof(text), HAL_MAX_DELAY);

	//enable LED GPIOs and set them into the right modes
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER &= ~(0xffff << 8);
	GPIOC->MODER |= 0x5555 << 8;

	//Turn LEDs on and off for a few seconds:
	for (int a = 0; a < 5; a++) {
		HAL_Delay(300);
		GPIOC->ODR |= 0xff << 4;
		HAL_Delay(300);
		GPIOC->ODR &= ~(0xff) << 4;
	}

	//shift one LED bit around
	for (int a = 0; a < 10; a++) {
		HAL_Delay(50);
		GPIOC->ODR &= ~(0xff) << 4;
		GPIOC->ODR |= 0x1 << 4;
		for (int b = 0; b < 8; b++) {
			HAL_Delay(50);
			GPIOC->ODR <<= 1;
		}
	}

}

void encoder_test(void) {
	char text[] = "Encoder test:\r\nTurn encoder to see increase / decrese.\r\nClick encoder to reset count.\r\nTest ends when PC0 button is pressed\r\n\r\n";

	//Enable pullups on encoder channels
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR8);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR8_0;

	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR9);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0;

	//Enable PC0 as an input button, active low, no pull-up / pull-down. Using on-board pull-up.
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER &= ~(0x1);

	//Enable LEDs for fun spinny time
	GPIOC->MODER &= ~(0xffff << 8);
	GPIOC->MODER |= 0x5555 << 8;
	GPIOC->ODR |= 0xff << 4;

	//Enable GPIOB for the encoder button and make sure PB15 is in input mode
	//No need to enable pull-up / pull-down since we already have a pull up on the board
	//Therefore, PB15 is will be active-low
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~(0x3 << 30);

	//Enable TIM1
	TIM1->CR1 |= TIM_CR1_CEN;

	TIM1->CNT = 0;

	HAL_UART_Transmit(&huart2, (unsigned char *)text, sizeof(text), HAL_MAX_DELAY);
	HAL_Delay(4000);

	char loop_text[35] = "";

	while (GPIOC->IDR & (0x1)) {
		HAL_Delay(10);

		//reset loop text and fill it with new values
		//for (int a = 0; a < sizeof(loop_text) / loop_text[0]; a++) loop_text[a] = '\0';
		snprintf(loop_text, sizeof(loop_text), "Encoder value: %5lu\r\n", TIM1->CNT);

		//Spin an LED around
		short leds = 0x1 << ((TIM1->CNT / 8) % 8);
		GPIOC->ODR &= ~(0xff << 4);
		GPIOC->ODR |= leds << 4;

		if(!(GPIOB->IDR & (0x1 << 15))) TIM1->CNT = 0;
		HAL_UART_Transmit(&huart2, (unsigned char *)loop_text, sizeof(loop_text), HAL_MAX_DELAY);
	}


}

void display_test() {

	display_setup();

	//Enable CS (active-low)
	GPIOB->ODR &= ~(GPIO_ODR_5);

	//Set display to double height font mode
	display_writecom(0x34);
	display_writecom(0x80);

	//write data to display
	uint8_t data[] = {' ', ' ', 'S','o', 'p', 'h', 'i', 'e', 'b', 'o', 'a', 'r', 'd', 0x10, ' ', ' '};
	for (int a = 0; a < (sizeof(data) / sizeof(data[0])); a++) {
		display_writedata((data[a]));
	}

	HAL_Delay(3000);

	//Back to single line font mode
	display_writecom(0x38);

	//Disable CS (active-low)
	GPIOB->ODR |= GPIO_ODR_5;


	display_print(0, "F0 Display Board");
	display_print(1, "By Michael Cupka");

}

void encoder_init() {

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	//Enable pullups on encoder channels
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR8);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR8_0;

	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR9);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0;

	//Enable PC0 as an input button, active low, no pull-up / pull-down. Using on-board pull-up.
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER &= ~(0x1);

	//Enable LEDs for fun spinny time
	GPIOC->MODER &= ~(0xffff << 8);
	GPIOC->MODER |= 0x5555 << 8;
	GPIOC->ODR |= 0xff << 4;

	//Enable GPIOB for the encoder button and make sure PB15 is in input mode
	//No need to enable pull-up / pull-down since we already have a pull up on the board
	//Therefore, PB15 is will be active-low
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~(0x3 << 30);

	//Enable TIM1
	TIM1->CR1 |= TIM_CR1_CEN;

	TIM1->CNT = 10000;

}

int encoder_navigate() {

	if (TIM1->CNT > 10000 + ENCODER_SENSITIVITY) {
			TIM1->CNT = 10000;
			GPIOC->ODR &= ~(0xff << 4);
			GPIOC->ODR |=(0x3 << 10);
			return 1;
		}
	else if (TIM1->CNT < 10000 - ENCODER_SENSITIVITY) {
			TIM1->CNT = 10000;
			GPIOC->ODR &= ~(0xff << 4);
			GPIOC->ODR |=(0x3 << 4);
			return -1;
	}
	else
		return 0;
}

int encoder_select() {

	if(!(GPIOB->IDR & (0x1 << 15))) {
		TIM1->CNT = 0;
		return 1;
	}
	else return 0;
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
