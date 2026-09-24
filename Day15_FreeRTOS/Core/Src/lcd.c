#include "lcd.h"

static I2C_HandleTypeDef *lcd_i2c;

static void LCD_SendCommand(uint8_t cmd);
static void LCD_SendData(uint8_t data);

void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;

    HAL_Delay(50);

    LCD_SendCommand(0x30);
    HAL_Delay(5);

    LCD_SendCommand(0x30);
    HAL_Delay(1);

    LCD_SendCommand(0x30);
    HAL_Delay(1);

    LCD_SendCommand(0x20);
    HAL_Delay(1);

    LCD_SendCommand(0x28);   // 4-bit, 2-line
    LCD_SendCommand(0x0C);   // Display ON, cursor OFF
    LCD_SendCommand(0x06);   // Entry mode
    LCD_SendCommand(0x01);   // Clear display

    HAL_Delay(2);
}

static void LCD_SendCommand(uint8_t cmd)
{
    uint8_t data[4];

    data[0] = (cmd & 0xF0) | 0x0C;
    data[1] = (cmd & 0xF0) | 0x08;
    data[2] = ((cmd << 4) & 0xF0) | 0x0C;
    data[3] = ((cmd << 4) & 0xF0) | 0x08;

    HAL_I2C_Master_Transmit(lcd_i2c, LCD_ADDR, data, 4, 100);
}

static void LCD_SendData(uint8_t data_byte)
{
    uint8_t data[4];

    data[0] = (data_byte & 0xF0) | 0x0D;
    data[1] = (data_byte & 0xF0) | 0x09;
    data[2] = ((data_byte << 4) & 0xF0) | 0x0D;
    data[3] = ((data_byte << 4) & 0xF0) | 0x09;

    HAL_I2C_Master_Transmit(lcd_i2c, LCD_ADDR, data, 4, 100);
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
        LCD_SendData((uint8_t)*str++);
    }
}
