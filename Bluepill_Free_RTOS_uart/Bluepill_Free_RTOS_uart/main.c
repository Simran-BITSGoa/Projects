#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>  // ? Fix: Include stdio.h for printf and FILE

// Function prototypes
void ledInit(void);
void uartInit(void);
void vTaskLedBlink(void *p);
void vTaskUartSend(void *p);
void setupSysTick(void);

// ? Fix: Explicitly declare fputc() prototype
int fputc(int ch, FILE *f);

int main(void)
{
    // Initialize hardware
    ledInit();
    uartInit();
    setupSysTick(); // Ensure SysTick is configured for FreeRTOS scheduling

    // Create FreeRTOS tasks with different priorities
    xTaskCreate(vTaskLedBlink, "LED Blink", 128, NULL, 2, NULL);  // Priority 2
    xTaskCreate(vTaskUartSend, "UART Send", 128, NULL, 1, NULL);  // Priority 1
    
    // Start RTOS scheduler
    vTaskStartScheduler();

    while (1);
}

// ? *LED GPIO Initialization*
void ledInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Enable Clock for GPIOC
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;  

    // Configure PC13 as push-pull output
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// ? *USART1 Initialization*
void uartInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Enable Clocks for GPIOA and USART1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // Enable GPIOA Clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enable USART1 Clock
    
    // Configure PA9 (USART1 TX) as Alternate Function Push-Pull
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure USART1
    USART1->BRR = 72000000 / 9600;  // Baud rate = 9600 (assuming 72MHz PCLK2)
    USART1->CR1 |= USART_CR1_TE;    // Enable Transmitter
    USART1->CR1 |= USART_CR1_UE;    // Enable USART1

    // Wait until USART1 is enabled properly
    while (!(USART1->CR1 & USART_CR1_UE));

    // Flush USART1 Status Registers
    USART1->SR = 0;
}

// ? *Setup SysTick Timer for FreeRTOS Task Switching*
void setupSysTick()
{
    SysTick_Config(SystemCoreClock / 1000);  // Generate 1ms time base
    NVIC_SetPriority(SysTick_IRQn, 5);  // Set lower priority for FreeRTOS compatibility
}

// ? *Task: Blink LED*
void vTaskLedBlink(void *p)
{
    while (1)
    {
        GPIOC->ODR ^= GPIO_Pin_13; // Toggle PC13
        printf("LED Blink Task Executed!\r\n"); // Debug message
        vTaskDelay(500 / portTICK_RATE_MS); // Delay 500ms
    }
}

// ? *Task: Send Character over USART*
void vTaskUartSend(void *p)
{
    while (1)
    {
        while (!(USART1->SR & USART_SR_TXE)); // Wait until TX buffer is empty
        USART1->DR = 'A'; // Send character
        
        printf("UART Task Executed!\r\n"); // Debug message
        vTaskDelay(1000 / portTICK_RATE_MS); // Delay 1 second
    }
}

// ? *Fix: Proper printf redirection function*
int fputc(int ch, FILE *f)
{
    while (!(USART1->SR & USART_SR_TXE)); // Wait for TX ready
    USART1->DR = (uint8_t)ch;
    return ch;
}