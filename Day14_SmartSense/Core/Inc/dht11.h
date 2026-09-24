#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"

typedef struct
{
    uint8_t temperature;
    uint8_t humidity;
} DHT11_Data;

void DHT11_Init(void);
uint8_t DHT11_Read(DHT11_Data *data);

#endif
