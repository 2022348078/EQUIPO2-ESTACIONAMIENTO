#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

void PWM_Init(void);
void Servo_SetPulse(uint16_t pulso);
void Barrera_Abrir(void);
void Barrera_Cerrar(void);

#endif
