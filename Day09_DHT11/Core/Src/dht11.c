#include "dht11.h"

extern TIM_HandleTypeDef htim1;

static void DHT11_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
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

static void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);

    while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}

static uint8_t wait_for_state(GPIO_PinState state, uint16_t timeout)
{
    uint16_t start = __HAL_TIM_GET_COUNTER(&htim1);

    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) != state)
    {
        if ((__HAL_TIM_GET_COUNTER(&htim1) - start) >= timeout)
        {
            return 0;
        }
    }

    return 1;
}

uint8_t DHT11_Read(uint8_t *temperature, uint8_t *humidity)
{
    uint8_t data[5] = {0};

    /* Start signal */

    DHT11_SetOutput();

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);

    HAL_Delay(20);

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);

    delay_us(30);

    DHT11_SetInput();

    /* DHT11 response */

    if (!wait_for_state(GPIO_PIN_RESET, 100))
        return 0;

    if (!wait_for_state(GPIO_PIN_SET, 100))
        return 0;

    if (!wait_for_state(GPIO_PIN_RESET, 100))
        return 0;

    /* Receive 40 bits */

    for (uint8_t i = 0; i < 40; i++)
    {
        /* Wait for start of bit */

        if (!wait_for_state(GPIO_PIN_SET, 100))
            return 0;

        /*
         * DHT11:
         *
         * 0 = HIGH for about 26-28 us
         * 1 = HIGH for about 70 us
         */

        delay_us(40);

        if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
        {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }

        /* Wait until LOW */

        if (!wait_for_state(GPIO_PIN_RESET, 100))
            return 0;
    }

    /* Checksum */

    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
    {
        return 0;
    }

    *humidity = data[0];
    *temperature = data[2];

    return 1;
}
