#ifndef LCD_H
#define LCD_H

#include "stm32f4xx.h"

void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *str);
void LCD_SetCursor(uint8_t row, uint8_t col);
void I2C1_WriteByte(uint8_t slaveAddr, uint8_t data);
void delayMs(int n);

#endif
