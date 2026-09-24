#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_

#include "main.h"

#define LCD_ADDR (0x27 << 1)

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_SendString(char *str);

#endif
