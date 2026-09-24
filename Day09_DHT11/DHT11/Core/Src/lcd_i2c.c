#include "lcd_i2c.h"

extern I2C_HandleTypeDef hi2c1;

static void LCD_SendInternal(uint8_t data, uint8_t rs)
{
    uint8_t high = data & 0xF0;
    uint8_t low  = (data << 4) & 0xF0;

    uint8_t data_arr[4];

    data_arr[0] = high | 0x08 | rs | 0x04;
    data_arr[1] = high | 0x08 | rs;
    data_arr[2] = low  | 0x08 | rs | 0x04;
    data_arr[3] = low  | 0x08 | rs;

    HAL_I2C_Master_Transmit(&hi2c1,
                            LCD_ADDR,
                            data_arr,
                            4,
                            HAL_MAX_DELAY);
}

static void LCD_SendCommand(uint8_t cmd)
{
    LCD_SendInternal(cmd, 0);
    HAL_Delay(2);
}

static void LCD_SendData(uint8_t data)
{
    LCD_SendInternal(data, 1);
}

void LCD_Init(void)
{
    HAL_Delay(50);

    LCD_SendCommand(0x30);
    HAL_Delay(5);

    LCD_SendCommand(0x30);
    HAL_Delay(1);

    LCD_SendCommand(0x30);
    HAL_Delay(1);

    LCD_SendCommand(0x20);
    HAL_Delay(1);

    LCD_SendCommand(0x28);
    LCD_SendCommand(0x08);
    LCD_SendCommand(0x01);
    HAL_Delay(2);
    LCD_SendCommand(0x06);
    LCD_SendCommand(0x0C);
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    if (row == 0)
        address = 0x80 + col;
    else
        address = 0xC0 + col;

    LCD_SendCommand(address);
}

void LCD_SendString(char *str)
{
    while (*str)
    {
        LCD_SendData(*str++);
    }
}
