#include <stdint.h>

#include "pwm.h"
#include "sensores.h"


/* Registro de salida GPIOD */
#define GPIOD_ODR (*(volatile uint32_t *)0x40020C14)


int main(void)
{
    /* Inicializar PWM */
    PWM_Init();

    /* Inicializar sensores y LEDs */
    Sensores_Init();

    /* Barrera inicialmente cerrada */
    Barrera_Cerrar();


    while(1)
    {
        /* ==================================
           SENSOR DE ENTRADA
           ================================== */

        if(Sensor_Entrada())
        {
            /* LED verde ON */
            GPIOD_ODR |= (1 << 12);

            /* Abrir barrera */
            Barrera_Abrir();
        }
        else
        {
            /* LED verde OFF */
            GPIOD_ODR &= ~(1 << 12);
        }


        /* ==================================
           SENSOR DE SALIDA
           ================================== */

        if(Sensor_Salida())
        {
            /* LED rojo ON */
            GPIOD_ODR |= (1 << 14);

            /* Cerrar barrera */
            Barrera_Cerrar();
        }
        else
        {
            /* LED rojo OFF */
            GPIOD_ODR &= ~(1 << 14);
        }
    }
}
