#include "stm32f4xx.h"
#include "LCD.h"

#define SLAVE_ADDR 0x27
// LCD I2C Address

void SystemClock_Config(void);
void I2C1_Init(void);
void I2C1_WriteByte(uint8_t slaveAddr, uint8_t data);
void delayMs(int n);

int main(void) {
    SystemClock_Config();
    I2C1_Init();

    LCD_Init();               
    LCD_SendString("HELLO");   
    LCD_SetCursor(1, 0);       
    LCD_SendString("WORLD!");  

    while (1) {
        // Infinite loop
    }
}

/* I2C1 Initialization */
void I2C1_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;    

    GPIOB->AFR[1] &= ~0x000000FFU;
    GPIOB->AFR[1] |=  0x00000044U;

    GPIOB->MODER &= ~0x000F0000U;
    GPIOB->MODER |=  0x000A0000U;

    GPIOB->OTYPER |= 0x00000300U;  
    GPIOB->PUPDR &= ~0x000F0000U;  
    GPIOB->PUPDR |=  0x00050000U;  

    I2C1->CR1 = I2C_CR1_SWRST;      
    I2C1->CR1 &= ~I2C_CR1_SWRST;    

    I2C1->CR2   = 16;       
    I2C1->CCR   = 80;       
    I2C1->TRISE = 17;       

    I2C1->CR1  |= I2C_CR1_PE; 
}

/* Send one byte over I2C */
void I2C1_WriteByte(uint8_t slaveAddr, uint8_t data) {
    while (I2C1->SR2 & I2C_SR2_BUSY);  

    I2C1->CR1 |= I2C_CR1_START;        
    while (!(I2C1->SR1 & I2C_SR1_SB)); 

    I2C1->DR = (slaveAddr << 1);       
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;                    

    while (!(I2C1->SR1 & I2C_SR1_TXE)); 
    I2C1->DR = data;                    

    while (!(I2C1->SR1 & I2C_SR1_BTF)); 
    I2C1->CR1 |= I2C_CR1_STOP;          
}

/* Delay function */
void delayMs(int n) {
    int i;
    for (; n > 0; n--) {
        for (i = 0; i < 3195; i++);
    }
}

/* System Clock Configuration */
void SystemClock_Config(void) {
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSE | RCC_PLLCFGR_PLLM_4 | (168 << 6) | RCC_PLLCFGR_PLLP_0 | (7 << 24));
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}
