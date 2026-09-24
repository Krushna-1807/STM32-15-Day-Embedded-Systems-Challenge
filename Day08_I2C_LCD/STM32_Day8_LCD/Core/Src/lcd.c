#include "lcd.h"

static I2C_HandleTypeDef *lcd_i2c;

static void LCD_SendInternal(uint8_t data, uint8_t mode)
{
    uint8_t high_nibble = data & 0xF0;
    uint8_t low_nibble  = (data << 4) & 0xF0;

    uint8_t data_arr[4];

    data_arr[0] = high_nibble | mode | 0x0C;
    data_arr[1] = high_nibble | mode | 0x08;
    data_arr[2] = low_nibble | mode | 0x0C;
    data_arr[3] = low_nibble | mode | 0x08;

    HAL_I2C_Master_Transmit(lcd_i2c, LCD_ADDR,
                            data_arr, 4, HAL_MAX_DELAY);
}

void LCD_SendCommand(uint8_t command)
{
    LCD_SendInternal(command, 0x00);
}

void LCD_SendData(uint8_t data)
{
    LCD_SendInternal(data, 0x01);
}

void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;

    HAL_Delay(50);

    LCD_SendCommand(0x30);
    HAL_Delay(5);

    LCD_SendCommand(0x30);
    HAL_Delay(1);

    LCD_SendCommand(0x30);
    HAL_Delay(10);

    LCD_SendCommand(0x20);
    HAL_Delay(10);

    LCD_SendCommand(0x28);  // 4-bit, 2-line
    LCD_SendCommand(0x08);  // Display OFF
    LCD_SendCommand(0x01);  // Clear display
    HAL_Delay(2);

    LCD_SendCommand(0x06);  // Entry mode
    LCD_SendCommand(0x0C);  // Display ON, cursor OFF
}

void LCD_SendString(char *str)
{
    while (*str)
    {
        LCD_SendData((uint8_t)*str);
        str++;
    }
}

void LCD_SetCursor(uint8_t row, uint8_t column)
{
    uint8_t address;

    if (row == 0)
        address = 0x80 + column;
    else
        address = 0xC0 + column;

    LCD_SendCommand(address);
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}
