#include "dht11.h"

extern TIM_HandleTypeDef htim1;

/* DHT11 DATA pin */
#define DHT11_PORT GPIOA
#define DHT11_PIN  GPIO_PIN_1

static void DHT11_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void DHT11_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void Delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);

    while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}

static uint8_t DHT11_ReadBit(void)
{
    uint32_t timeout = 0;

    /* Wait for signal to become HIGH */
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET)
    {
        if (++timeout > 100)
            return 0;
    }

    /* Wait approximately 40 us */
    Delay_us(40);

    if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
    {
        /* Wait until signal goes LOW */
        timeout = 0;

        while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
        {
            if (++timeout > 100)
                break;
        }

        return 1;
    }

    return 0;
}

void DHT11_Init(void)
{
    DHT11_SetOutput();

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
}

uint8_t DHT11_Read(DHT11_Data *data)
{
    uint8_t bits[5] = {0};

    /* Start signal */
    DHT11_SetOutput();

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);

    HAL_Delay(18);

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);

    Delay_us(20);

    /* Change to input */
    DHT11_SetInput();

    /* Wait for DHT11 response */

    uint32_t timeout = 0;

    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
    {
        if (++timeout > 1000)
            return 0;
    }

    timeout = 0;

    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET)
    {
        if (++timeout > 1000)
            return 0;
    }

    timeout = 0;

    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
    {
        if (++timeout > 1000)
            return 0;
    }

    /* Read 40 bits */
    for (int i = 0; i < 40; i++)
    {
        uint8_t bit = DHT11_ReadBit();

        bits[i / 8] <<= 1;
        bits[i / 8] |= bit;
    }

    /* Checksum */
    uint8_t checksum =
            bits[0] +
            bits[1] +
            bits[2] +
            bits[3];

    if (checksum != bits[4])
    {
        return 0;
    }

    data->humidity = bits[0];
    data->temperature = bits[2];

    return 1;
}
