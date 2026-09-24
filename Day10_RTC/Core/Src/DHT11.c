#include "DHT11.h"

extern TIM_HandleTypeDef htim2;

#define DHT11_PORT GPIOA
#define DHT11_PIN  GPIO_PIN_1

static void DHT11_Set_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void DHT11_Set_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);

    while (__HAL_TIM_GET_COUNTER(&htim2) < us);
}

static uint32_t DHT11_Wait(GPIO_PinState state, uint32_t timeout)
{
    uint32_t start = __HAL_TIM_GET_COUNTER(&htim2);

    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) != state)
    {
        if ((__HAL_TIM_GET_COUNTER(&htim2) - start) > timeout)
        {
            return 0;
        }
    }

    return 1;
}

void DHT11_Init(void)
{
    DHT11_Set_Output();

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
}

uint8_t DHT11_Read(float *temperature, float *humidity)
{
    uint8_t data[5] = {0};

    /* Start signal */
    DHT11_Set_Output();

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);

    HAL_Delay(20);

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);

    delay_us(30);

    DHT11_Set_Input();

    /* DHT11 response:
       LOW ~80us
       HIGH ~80us
    */

    if (!DHT11_Wait(GPIO_PIN_RESET, 100))
        return 0;

    if (!DHT11_Wait(GPIO_PIN_SET, 100))
        return 0;

    if (!DHT11_Wait(GPIO_PIN_RESET, 100))
        return 0;

    /* Read 40 bits */
    for (uint8_t i = 0; i < 40; i++)
    {
        /* Wait for beginning of HIGH pulse */
        if (!DHT11_Wait(GPIO_PIN_SET, 100))
            return 0;

        /* Start measuring HIGH pulse */
        __HAL_TIM_SET_COUNTER(&htim2, 0);

        /* Wait until HIGH pulse ends */
        if (!DHT11_Wait(GPIO_PIN_RESET, 100))
            return 0;

        uint32_t pulse_time = __HAL_TIM_GET_COUNTER(&htim2);

        data[i / 8] <<= 1;

        /* ~26us = 0
           ~70us = 1 */
        if (pulse_time > 40)
        {
            data[i / 8] |= 1;
        }
    }

    /* Checksum */
    if ((uint8_t)(data[0] + data[1] +
                  data[2] + data[3]) != data[4])
    {
        return 0;
    }

    *humidity = data[0] + data[1] * 0.1f;
    *temperature = data[2] + data[3] * 0.1f;

    return 1;
}
