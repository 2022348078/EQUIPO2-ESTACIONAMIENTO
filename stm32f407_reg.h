#ifndef STM32F407_REG_H
#define STM32F407_REG_H

#include <stdint.h>

/* =========================================================
   RCC
   ========================================================= */

#define RCC_BASE        0x40023800UL

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40))


/* =========================================================
   GPIOB
   PB6 = Servo / TIM4_CH1
   ========================================================= */

#define GPIOB_BASE      0x40020400UL

#define GPIOB_MODER     (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_AFRL      (*(volatile uint32_t *)(GPIOB_BASE + 0x20))


/* =========================================================
   GPIOD
   LEDs STM32F407 Discovery
   ========================================================= */

#define GPIOD_BASE      0x40020C00UL

#define GPIOD_MODER     (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_ODR       (*(volatile uint32_t *)(GPIOD_BASE + 0x14))


/* =========================================================
   GPIOE
   PE0 = Sensor entrada
   PE1 = Sensor salida
   ========================================================= */

#define GPIOE_BASE      0x40021000UL

#define GPIOE_MODER     (*(volatile uint32_t *)(GPIOE_BASE + 0x00))
#define GPIOE_PUPDR     (*(volatile uint32_t *)(GPIOE_BASE + 0x0C))
#define GPIOE_IDR       (*(volatile uint32_t *)(GPIOE_BASE + 0x10))


/* =========================================================
   TIMER 4
   ========================================================= */

#define TIM4_BASE       0x40000800UL

#define TIM4_CR1        (*(volatile uint32_t *)(TIM4_BASE + 0x00))
#define TIM4_EGR        (*(volatile uint32_t *)(TIM4_BASE + 0x14))
#define TIM4_CCMR1      (*(volatile uint32_t *)(TIM4_BASE + 0x18))
#define TIM4_CCER       (*(volatile uint32_t *)(TIM4_BASE + 0x20))
#define TIM4_PSC        (*(volatile uint32_t *)(TIM4_BASE + 0x28))
#define TIM4_ARR        (*(volatile uint32_t *)(TIM4_BASE + 0x2C))
#define TIM4_CCR1       (*(volatile uint32_t *)(TIM4_BASE + 0x34))


/* =========================================================
   SYSTICK
   ========================================================= */

#define SYSTICK_CTRL    (*(volatile uint32_t *)0xE000E010UL)
#define SYSTICK_LOAD    (*(volatile uint32_t *)0xE000E014UL)
#define SYSTICK_VAL     (*(volatile uint32_t *)0xE000E018UL)


/* =========================================================
   FPU - CPACR
   ========================================================= */

#define SCB_CPACR       (*(volatile uint32_t *)0xE000ED88UL)

#endif
