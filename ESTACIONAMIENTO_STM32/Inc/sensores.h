#ifndef SENSORES_H_
#define SENSORES_H_

#include <stdint.h>

void Sensores_Init(void);

uint8_t Sensor_Entrada(void);
uint8_t Sensor_Salida(void);

#endif /* SENSORES_H_ */
