#include "stm32f4xx.h" 

#define RS 0x20  /* PB5 mask for reg select */ 
#define EN 0x80  /* PB7 mask for enable */

void delayMs(int n); 
void LCD_nibble_write(char data, unsigned char control); 
void LCD_command(unsigned char command); 
void LCD_data(char data); 
void LCD_init(void); 
void PORTS_init(void);

int main(void) { 
    /* Initialize LCD controller */ 
    LCD_init();
    
    while(1) { 
        /* Write "HELLO" on LCD */ 
        LCD_data('H'); 
        LCD_data('E'); 
        LCD_data('L'); 
        LCD_data('L'); 
        LCD_data('O'); 
        delayMs(1000);
        
        /* Clear LCD display */ 
        LCD_command(1); 
        delayMs(1000);
    } 
}

/* Initialize GPIOB/C then initialize LCD controller */ 
void LCD_init(void) { 
    PORTS_init();
    delayMs(20); /* LCD controller reset sequence */

    LCD_nibble_write(0x30, 0); 
    delayMs(5); 
    LCD_nibble_write(0x30, 0); 
    delayMs(1); 
    LCD_nibble_write(0x30, 0); 
    delayMs(1); 

    LCD_nibble_write(0x20, 0); /* Use 4-bit data mode */ 
    delayMs(1); 
    LCD_command(0x28); /* Set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x06); /* Move cursor right */ 
    LCD_command(0x01); /* Clear screen, move cursor to home */
    LCD_command(0x0F); /* Turn on display, cursor blinking */
}

void PORTS_init(void) {
    /* Enable GPIOB/C clock */
    RCC->AHB1ENR |= 0x06; 

    /* Configure PORTB for LCD R/S and EN */
    GPIOB->MODER &= ~0x0000CC00; /* Clear pin mode */ 
    GPIOB->MODER |= 0x00004400;  /* Set pin output mode */
    GPIOB->BSRR = 0x00800000;    /* Turn off EN */

    /* Configure PC4-PC7 for LCD D4-D7 */
    GPIOC->MODER &= ~0x0000FF00; /* Clear pin mode */ 
    GPIOC->MODER |= 0x00005500;  /* Set pin output mode */
}

void LCD_nibble_write(char data, unsigned char control) { 
    /* Populate data bits */ 
    GPIOC->BSRR = 0x00F00000; /* Clear data bits */ 
    GPIOC->BSRR = data & 0xF0; /* Set data bits */

    /* Set R/S bit */ 
    if (control & RS)
        GPIOB->BSRR = RS; 
    else 
        GPIOB->BSRR = RS << 16;

    /* Pulse E */ 
    GPIOB->BSRR = EN; 
    delayMs(0); 
    GPIOB->BSRR = EN << 16;
}

void LCD_command(unsigned char command) { 
    LCD_nibble_write(command & 0xF0, 0); /* Upper nibble first */
    LCD_nibble_write(command << 4, 0); /* Then lower nibble */

    if (command < 4) 
        delayMs(2); /* Commands 1 and 2 need up to 1.64ms */ 
    else 
        delayMs(1); /* All others 40us */ 
}

void LCD_data(char data) { 
    LCD_nibble_write(data & 0xF0, RS); /* Upper nibble first */
    LCD_nibble_write(data << 4, RS); /* Then lower nibble */
    delayMs(1); 
}

/* Delay function (16 MHz CPU clock) */ 
void delayMs(int n) { 
    int i; 
    for (; n > 0; n--) 
        for (i = 0; i < 3195; i++);
}
