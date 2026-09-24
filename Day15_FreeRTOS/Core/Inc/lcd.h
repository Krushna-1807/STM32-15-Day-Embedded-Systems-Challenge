#ifndef LCD_H
#define LCD_H

#include "main.h"

#define LCD_ADDR (0x27 << 1)

void LCD_Init(I2C_HandleTypeDef *hi2c);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_SendString(char *str);

#endif
