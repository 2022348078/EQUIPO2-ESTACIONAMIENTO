#include "sensores.h"

/* =========================
   RCC
   ========================= */

#define RCC_AHB1ENR   (*(volatile uint32_t *)0x40023830)


/* =========================
   GPIOE - SENSORES
   ========================= */

#define GPIOE_MODER   (*(volatile uint32_t *)0x40021000)
#define GPIOE_PUPDR   (*(volatile uint32_t *)0x4002100C)
#define GPIOE_IDR     (*(volatile uint32_t *)0x40021010)


/* =========================
   GPIOD - LEDS
   ========================= */

#define GPIOD_MODER   (*(volatile uint32_t *)0x40020C00)
#define GPIOD_ODR     (*(volatile uint32_t *)0x40020C14)


void Sensores_Init(void)
{
    /* Activar reloj GPIOE y GPIOD */
    RCC_AHB1ENR |= (1 << 4);   // GPIOE
    RCC_AHB1ENR |= (1 << 3);   // GPIOD


    /* =========================
       SENSORES
       ========================= */

    /* PE0 como entrada */
    GPIOE_MODER &= ~(3 << 0);

    /* PE1 como entrada */
    GPIOE_MODER &= ~(3 << 2);


    /* Pull-Up PE0 */
    GPIOE_PUPDR &= ~(3 << 0);
    GPIOE_PUPDR |=  (1 << 0);


    /* Pull-Up PE1 */
    GPIOE_PUPDR &= ~(3 << 2);
    GPIOE_PUPDR |=  (1 << 2);


    /* =========================
       LED VERDE - PD12
       ========================= */

    GPIOD_MODER &= ~(3 << 24);
    GPIOD_MODER |=  (1 << 24);


    /* =========================
       LED ROJO - PD14
       ========================= */

    GPIOD_MODER &= ~(3 << 28);
    GPIOD_MODER |=  (1 << 28);


    /* Empezar con LEDs apagados */
    GPIOD_ODR &= ~(1 << 12);
    GPIOD_ODR &= ~(1 << 14);
}


uint8_t Sensor_Entrada(void)
{
    /*
     * FC-51 normalmente:
     * 0 = objeto detectado
     * 1 = libre
     */

    if((GPIOE_IDR & (1 << 0)) == 0)
    {
        return 1;
    }

    return 0;
}


uint8_t Sensor_Salida(void)
{
    if((GPIOE_IDR & (1 << 1)) == 0)
    {
        return 1;
    }

    return 0;
}
