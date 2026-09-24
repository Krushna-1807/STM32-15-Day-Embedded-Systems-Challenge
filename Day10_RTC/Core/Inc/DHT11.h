#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"

void DHT11_Init(void);
uint8_t DHT11_Read(float *temperature, float *humidity);

#endif
