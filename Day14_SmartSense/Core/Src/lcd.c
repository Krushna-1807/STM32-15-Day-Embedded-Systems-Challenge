#include "lcd.h"

extern I2C_HandleTypeDef hi2c1;

/* PCF8574 control bits */
#define LCD_RS 0x01
#define LCD_EN 0x04
#define LCD_BL 0x08

static void LCD_Write4Bits(uint8_t data)
{
    uint8_t data_with_backlight = data | LCD_BL;

    HAL_I2C_Master_Transmit(&hi2c1,
                            LCD_I2C_ADDR,
                            &data_with_backlight,
                            1,
                            HAL_MAX_DELAY);

    data_with_backlight |= LCD_EN;

    HAL_I2C_Master_Transmit(&hi2c1,
                            LCD_I2C_ADDR,
                            &data_with_backlight,
                            1,
                            HAL_MAX_DELAY);

    HAL_Delay(1);

    data_with_backlight &= ~LCD_EN;

    HAL_I2C_Master_Transmit(&hi2c1,
                            LCD_I2C_ADDR,
                            &data_with_backlight,
                            1,
                            HAL_MAX_DELAY);

    HAL_Delay(1);
}

void LCD_SendCommand(uint8_t cmd)
{
    uint8_t high_nibble = cmd & 0xF0;
    uint8_t low_nibble = (cmd << 4) & 0xF0;

    LCD_Write4Bits(high_nibble);
    LCD_Write4Bits(low_nibble);

    HAL_Delay(2);
}

void LCD_SendData(uint8_t data)
{
    uint8_t high_nibble = data & 0xF0;
    uint8_t low_nibble = (data << 4) & 0xF0;

    LCD_Write4Bits(high_nibble | LCD_RS);
    LCD_Write4Bits(low_nibble | LCD_RS);
}

void LCD_Init(void)
{
    HAL_Delay(50);

    /* Force LCD into 8-bit mode */
    LCD_Write4Bits(0x30);
    HAL_Delay(5);

    LCD_Write4Bits(0x30);
    HAL_Delay(1);

    LCD_Write4Bits(0x30);
    HAL_Delay(10);

    /* Switch to 4-bit mode */
    LCD_Write4Bits(0x20);
    HAL_Delay(10);

    /* 4-bit, 2-line, 5x8 font */
    LCD_SendCommand(0x28);

    /* Display ON, cursor OFF */
    LCD_SendCommand(0x0C);

    /* Entry mode */
    LCD_SendCommand(0x06);

    /* Clear display */
    LCD_SendCommand(0x01);

    HAL_Delay(5);
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t column)
{
    uint8_t address;

    if (row == 0)
    {
        address = 0x80 + column;
    }
    else
    {
        address = 0xC0 + column;
    }

    LCD_SendCommand(address);
}

void LCD_Print(char *str)
{
    while (*str)
    {
        LCD_SendData(*str++);
    }
}
