#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"

#define LCD_I2C_ADDR (0x27 << 1)

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t column);
void LCD_Print(char *str);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);

#endif
