/***************************************************************************//**
*   \file       main.c
*
*   \details    Blinking Onboard LED with some random delay - STM32 GPIO Tutorial
*
*   \author     Ajay  (14/Feb/2025)
*
*   \This code is verified with STM32F103CBT6 Board - Working FreeRTOS used is 7.3
*
*******************************************************************************/

/***************************************************************************//**

		\details  Providing task for leds

		\return   void

		\retval   none

	*******************************************************************************/
/*	static void delay( void )
	{
			uint32_t i = 0;
			for( i=0; i<=DELAY_COUNT; i++ );
	} */

	/***************************************************************************//**

		\details  The main function.  It should not return.

		\return   void

		\retval   none

	*******************************************************************************/


#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "FreeRTOS/include/FreeRTOS.h" // Sorry Raju :P ! but the idea for hard path cannot be c:/abc/pqr 
#include "FreeRTOS/include/task.h"  

void ledInit(void);
// RTOS task
void vTaskLedRed(void *p);
void vTaskLedYellow(void *p);
void vTaskLedGreen(void *p);

int main(void)
{
	// Configure GPIO for LED
	ledInit();
		
	// Create LED blink task
	xTaskCreate(vTaskLedRed, (const char*) "Red LED Blink", 128, NULL, 1, NULL);
	xTaskCreate(vTaskLedYellow, (const char*) "Yellow LED Blink", 128, NULL, 1, NULL);
	xTaskCreate(vTaskLedGreen, (const char*) "Green LED Blink", 128, NULL, 1, NULL);
	// Start RTOS scheduler
  vTaskStartScheduler();
	
	return 0;
}

void ledInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// Configure PC13, PC14, PC15 as push-pull output
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void vTaskLedRed(void *p)
{
	for (;;)
	{
		GPIOC->ODR ^= GPIO_Pin_13;
		vTaskDelay(100/portTICK_RATE_MS);
	}
}

void vTaskLedYellow(void *p)
{
	for (;;)
	{
		GPIOC->ODR ^= GPIO_Pin_15;
		vTaskDelay(500/portTICK_RATE_MS);
	}
}

void vTaskLedGreen(void *p)
{
	for (;;)
	{
		GPIOC->ODR ^= GPIO_Pin_14;
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}
