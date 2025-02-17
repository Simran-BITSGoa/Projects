/**
  ******************************************************************************
  * @file    main.c
  * @brief   FreeRTOS tasks on STM32F401RE (HSI -> 84 MHz), UART @ 9600.
  ******************************************************************************
  */

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

/* Forward declarations ----------------------------------------------------- */
void SystemClock_Config(void);
void UART_Init(void);
void vTask1(void *pvParams);
void vTask2(void *pvParams);

/* Global handle for UART2 ------------------------------------------------- */
UART_HandleTypeDef huart2;

/* --------------------------------------------------------------------------
   MAIN FUNCTION
   -------------------------------------------------------------------------- */
int main(void)
{
    /* Initialize the HAL library */
    HAL_Init();

    /*
       Configure the system clock:
       - Internal HSI = 16 MHz
       - PLL -> 84 MHz system clock
       - APB1 = 42 MHz, APB2 = 84 MHz
    */
    SystemClock_Config();

    /* Initialize UART2 on PA2 (TX), PA3 (RX) at 9600 baud */
    UART_Init();

    /* Enable GPIOA clock for LED on PA5 */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure PA5 as output for on-board LED */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = GPIO_PIN_5;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Create two FreeRTOS tasks */
    xTaskCreate(vTask1, "Task1", 128, NULL, 1, NULL);
    xTaskCreate(vTask2, "Task2", 128, NULL, 1, NULL);

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    /* Should never reach here if FreeRTOS is running */
    while (1)
    {
    }
}

/* --------------------------------------------------------------------------
   TASK 1
   Sends a message over UART and turns LED (PA5) ON.
   -------------------------------------------------------------------------- */
void vTask1(void *pvParams)
{
    (void) pvParams;   // Unused
    const char *msg = "Task-1 Running.\r\n";

    for (;;)
    {
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

        // Turn LED ON
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

        // Simple blocking delay
        for (volatile uint32_t i = 0; i < 3000000; i++);

        // Yield to Task2
        taskYIELD();
    }
}

/* --------------------------------------------------------------------------
   TASK 2
   Sends a message over UART and turns LED (PA5) OFF.
   -------------------------------------------------------------------------- */
void vTask2(void *pvParams)
{
    (void) pvParams;
    const char *msg = "Task-2 Running.\r\n";

    for (;;)
    {
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

        // Turn LED OFF
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

        // Simple blocking delay
        for (volatile uint32_t i = 0; i < 3000000; i++);

        // Yield to Task1
        taskYIELD();
    }
}

/* --------------------------------------------------------------------------
   UART Initialization (USART2 @ 9600 8N1)
   - PA2 = TX, PA3 = RX, AF7
   -------------------------------------------------------------------------- */
void UART_Init(void)
{
    /* Enable USART2 and GPIOA clocks */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure PA2 (TX), PA3 (RX) as Alternate Function for USART2 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2; // AF7 for USART2 on STM32F4
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure UART peripheral */
    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = 9600;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }
}

/* --------------------------------------------------------------------------
   System Clock Configuration (for STM32F401RE with HSI -> 84 MHz)
   -------------------------------------------------------------------------- */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* 1) Enable power clock, set voltage regulator scale */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /* 2) Configure HSI as 16 MHz internal clock */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState           = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue= RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState       = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource      = RCC_PLLSOURCE_HSI;

    /*
       PLLM = 16 => 16 / 16 = 1 MHz input
       PLLN = 336 => 1 * 336 = 336 MHz VCO
       PLLP = 4 => 336 / 4 = 84 MHz SYSCLK
       PLLQ = 7 => 336 / 7 = 48 MHz for USB if needed
    */
    RCC_OscInitStruct.PLL.PLLM           = 16;
    RCC_OscInitStruct.PLL.PLLN           = 336;
    RCC_OscInitStruct.PLL.PLLP           = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ           = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while (1);
    }

    /* 3) Configure clock dividers: AHB=84 MHz, APB1=42 MHz, APB2=84 MHz */
    RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK  |
                                        RCC_CLOCKTYPE_HCLK    |
                                        RCC_CLOCKTYPE_PCLK1   |
                                        RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;  // 84 MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;    // 42 MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;    // 84 MHz

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        while (1);
    }
}
