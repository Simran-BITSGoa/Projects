/***************************************************************************//**
*   \file       main.c
*
*   \details    Blinking Onboard LED with some random delay - STM32 GPIO Tutorial
*
*   \author     Ajay  (14/Feb/2025)
*
*   \This code is verified with STM32F103CBT6 Board - Working
*
*******************************************************************************/

/***************************************************************************//**

		\details  Providing Delay by running empty for loop

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

#define DELAY_COUNT     ( 800000 )    /* Adjust as needed */

static void delay( void )
{
    uint32_t i = 0;
    for( i=0; i<=DELAY_COUNT; i++ );
}

int main(void)
{
    // Enable the RCC clock for GPIO Port A
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA5 as output
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5); // Clear mode and config
    GPIOA->CRL |= GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0; // Set mode to output (50MHz max speed)

    while(1)
    {
        // Turn ON the LED (PA5)
        GPIOA->BSRR = GPIO_BSRR_BS5;

        delay();

        // Turn OFF the LED (PA5)
        GPIOA->BSRR = GPIO_BSRR_BR5;

        delay();
    }
}
