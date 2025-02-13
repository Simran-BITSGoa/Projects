#include "LCD.h"

#define LCD_I2C_ADDR 0x27  // LCD I2C address (check with 0x27 or 0x3F)

void LCD_Init(void) {
    delayMs(50);
    LCD_SendCommand(0x33);  // Initialize LCD for 4-bit mode
    LCD_SendCommand(0x32);
    LCD_SendCommand(0x28);  // 4-bit, 2 lines, 5x8 dots
    LCD_SendCommand(0x0C);  // Display ON, Cursor OFF
    LCD_SendCommand(0x06);  // Auto Increment Cursor
    LCD_SendCommand(0x01);  // Clear Display
    delayMs(2);
}

void LCD_SendCommand(uint8_t cmd) {
    uint8_t data_u = (cmd & 0xF0); // Upper nibble
    uint8_t data_l = ((cmd << 4) & 0xF0); // Lower nibble

    I2C1_WriteByte(LCD_I2C_ADDR, data_u | 0x08);
    I2C1_WriteByte(LCD_I2C_ADDR, data_u);
    I2C1_WriteByte(LCD_I2C_ADDR, data_l | 0x08);
    I2C1_WriteByte(LCD_I2C_ADDR, data_l);

    delayMs(1);
}

void LCD_SendData(uint8_t data) {
    uint8_t data_u = (data & 0xF0);
    uint8_t data_l = ((data << 4) & 0xF0);

    I2C1_WriteByte(LCD_I2C_ADDR, data_u | 0x0D);
    I2C1_WriteByte(LCD_I2C_ADDR, data_u | 0x09);
    I2C1_WriteByte(LCD_I2C_ADDR, data_l | 0x0D);
    I2C1_WriteByte(LCD_I2C_ADDR, data_l | 0x09);

    delayMs(1);
}

void LCD_SendString(char *str) {
    while (*str) {
        LCD_SendData(*str);
        str++;
    }
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_SendCommand(addr);
}
