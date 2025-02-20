/*************************************************************************
 * USART1 TX/RX Example with LED Blink on TX Only (STM32F103C8)
 *
 * - Sends 'A' repeatedly at 9600 baud.
 * - Receives data via USART1_RX and stores it (No LED blink on RX).
 * - LED blinks only when transmitting.
 *
 * Pin:
 *   PA9  => USART1_TX
 *   PA10 => USART1_RX
 *   PC13 => On-board LED (Blinks only on TX)
 *
 * Clock:
 *   8 MHz HSE + x9 PLL => 72 MHz system clock.
 *************************************************************************/

#include "stm32f10x.h"

#define RX_BUF_SIZE 64
volatile char rxBuffer[RX_BUF_SIZE];  // Receive buffer
volatile uint8_t rxIndex = 0;         // RX buffer index

void USART1_Init(void);
void delayMs(uint32_t ms);
void USART1_IRQHandler(void);

int main(void)
{
    /************ 1) Enable Clocks ************/
    RCC->APB2ENR |= (1U << 2);  // Enable GPIOA (USART TX/RX)
    RCC->APB2ENR |= (1U << 4);  // Enable GPIOC (LED)
    RCC->APB2ENR |= (1U << 0);  // Enable AFIO
    RCC->APB2ENR |= (1U << 14); // Enable USART1

    /************ 2) Configure PA9 (TX) ************/
    GPIOA->CRH &= ~(0xF << (4 * 1));  // Clear CNF/MODE for PA9
    GPIOA->CRH |=  (0xB << (4 * 1));  // PA9: AF Push-Pull, 50 MHz

    /************ 3) Configure PA10 (RX) ************/
    GPIOA->CRH &= ~(0xF << (4 * 2));  // Clear CNF/MODE for PA10
    GPIOA->CRH |=  (0x4 << (4 * 2));  // PA10: Floating Input

    /************ 4) Configure PC13 (LED) ************/
    GPIOC->CRH &= ~(0xF << (4 * 5));  // Clear CNF/MODE for PC13
    GPIOC->CRH |=  (0x1 << (4 * 5));  // PC13: Output Push-Pull, 10 MHz

    /************ 5) Initialize USART1 ************/
    USART1_Init();

    /************ 6) Main Loop ************/
    while(1)
    {
        /* ? Transmit 'A' */
        while (!(USART1->SR & (1U << 7))) {}  // Wait until TXE=1
        USART1->DR = 'B';                     // Send 'A'
        GPIOC->ODR ^= (1U << 13);             // Toggle LED on TX

        /* ? Delay for Visibility */
        delayMs(500);
    }
}

/*************************************************************************
 * USART1 Initialization (9600 Baud, 8N1, Interrupt RX)
 *************************************************************************/
void USART1_Init(void)
{
    /* 1) Set Baud Rate: 9600 at 72 MHz */
    USART1->BRR = 0x1D4C;  // 9600 baud @ 72 MHz

    /* 2) Enable USART1 TX & RX */
    USART1->CR1 |= (1U << 3); // TE = 1 (Enable TX)
    USART1->CR1 |= (1U << 2); // RE = 1 (Enable RX)

    /* 3) Enable USART1 Interrupt for RX */
    USART1->CR1 |= (1U << 5); // RXNEIE = 1 (RX interrupt enable)
    NVIC_EnableIRQ(USART1_IRQn); // Enable IRQ in NVIC

    /* 4) Enable USART1 */
    USART1->CR1 |= (1U << 13); // UE = 1 (Enable USART)
}

/*************************************************************************
 * USART1 IRQ Handler - Receives Data and Stores in Buffer (No LED Blink)
 *************************************************************************/
void USART1_IRQHandler(void)
{
    if (USART1->SR & (1U << 5))  // RXNE = 1 (Data received)
    {
        rxBuffer[rxIndex++] = USART1->DR;  // Read received data
        if (rxIndex >= RX_BUF_SIZE) rxIndex = 0;  // Reset index
    }
}

/*************************************************************************
 * delayMs: Blocking delay ~1 ms per 'ms' at 72 MHz
 *************************************************************************/
void delayMs(uint32_t ms)
{
    volatile uint32_t i;
    for (; ms > 0; ms--)
    {
        for (i = 0; i < 7200; i++)
        {
            __NOP(); // No Operation (prevents compiler optimization)
        }
    }
}
