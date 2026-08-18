#include "pwm.h"

/* =========================
   REGISTROS RCC
   ========================= */

#define RCC_AHB1ENR   (*(volatile uint32_t *)0x40023830)
#define RCC_APB1ENR   (*(volatile uint32_t *)0x40023840)


/* =========================
   REGISTROS GPIOB
   ========================= */

#define GPIOB_MODER   (*(volatile uint32_t *)0x40020400)
#define GPIOB_AFRL    (*(volatile uint32_t *)0x40020420)


/* =========================
   REGISTROS TIM4
   ========================= */

#define TIM4_CR1      (*(volatile uint32_t *)0x40000800)
#define TIM4_EGR      (*(volatile uint32_t *)0x40000814)
#define TIM4_CCMR1    (*(volatile uint32_t *)0x40000818)
#define TIM4_CCER     (*(volatile uint32_t *)0x40000820)
#define TIM4_PSC      (*(volatile uint32_t *)0x40000828)
#define TIM4_ARR      (*(volatile uint32_t *)0x4000082C)
#define TIM4_CCR1     (*(volatile uint32_t *)0x40000834)


/* =========================
   INICIALIZACIÓN DEL PWM
   ========================= */

void PWM_Init(void)
{
    /* 1. Activar reloj de GPIOB */
    RCC_AHB1ENR |= (1 << 1);

    /* 2. Activar reloj de TIM4 */
    RCC_APB1ENR |= (1 << 2);


    /* =========================
       CONFIGURAR PB6
       ========================= */

    /* PB6 como Alternate Function */
    GPIOB_MODER &= ~(3 << 12);
    GPIOB_MODER |=  (2 << 12);

    /* PB6 = AF2 = TIM4_CH1 */
    GPIOB_AFRL &= ~(0xF << 24);
    GPIOB_AFRL |=  (2 << 24);


    /* =========================
       CONFIGURAR TIM4
       ========================= */

    /*
     * Suponiendo TIM4 = 84 MHz:
     *
     * 84 MHz / (83 + 1) = 1 MHz
     *
     * Por lo tanto:
     * 1 cuenta = 1 us
     */

    TIM4_PSC = 15;


    /*
     * 20,000 cuentas = 20 ms
     *
     * Frecuencia:
     * 1 / 0.020 = 50 Hz
     */

    TIM4_ARR = 19999;


    /* =========================
       PWM MODE 1
       ========================= */

    /* Limpiar OC1M */
    TIM4_CCMR1 &= ~(7 << 4);

    /* PWM Mode 1 */
    TIM4_CCMR1 |= (6 << 4);

    /* Habilitar preload de CCR1 */
    TIM4_CCMR1 |= (1 << 3);


    /* =========================
       HABILITAR CHANNEL 1
       ========================= */

    TIM4_CCER |= (1 << 0);


    /* =========================
       POSICIÓN INICIAL
       ========================= */

    /* 1000 us = barrera cerrada */
    TIM4_CCR1 = 1000;


    /* Forzar actualización del timer */
    TIM4_EGR |= (1 << 0);


    /* Encender TIM4 */
    TIM4_CR1 |= (1 << 0);
}


/* =========================
   CONTROL DEL SERVO
   ========================= */

void Servo_SetPulse(uint16_t pulso)
{
    /*
     * Cambia el ancho del pulso PWM.
     *
     * Aproximadamente:
     *
     * 1000 us = extremo
     * 1500 us = centro
     * 2000 us = extremo contrario
     */

    TIM4_CCR1 = pulso;
}


/* =========================
   ABRIR BARRERA
   ========================= */

void Barrera_Abrir(void)
{
    Servo_SetPulse(1000);
}

void Barrera_Cerrar(void)
{
    Servo_SetPulse(1500);
}
