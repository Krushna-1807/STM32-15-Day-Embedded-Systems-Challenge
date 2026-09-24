#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"

#define LCD_ADDR (0x27 << 1)

void LCD_Init(I2C_HandleTypeDef *hi2c);
void LCD_SendCommand(uint8_t command);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *str);
void LCD_SetCursor(uint8_t row, uint8_t column);
void LCD_Clear(void);

#endif /* INC_LCD_H_ */
