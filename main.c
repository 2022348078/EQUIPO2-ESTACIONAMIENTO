#include <stdint.h>

#define RCC_BASE       0x40023800UL
#define RCC_CR         (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR       (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_AHB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_APB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x40))

#define GPIOA_BASE     0x40020000UL
#define GPIOA_MODER    (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_IDR      (*(volatile uint32_t *)(GPIOA_BASE + 0x10))
#define GPIOA_ODR      (*(volatile uint32_t *)(GPIOA_BASE + 0x14))

#define GPIOB_BASE     0x40020400UL
#define GPIOB_MODER    (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_AFRL     (*(volatile uint32_t *)(GPIOB_BASE + 0x20))

#define GPIOD_BASE     0x40020C00UL
#define GPIOD_MODER    (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_ODR      (*(volatile uint32_t *)(GPIOD_BASE + 0x14))

#define GPIOE_BASE     0x40021000UL
#define GPIOE_MODER    (*(volatile uint32_t *)(GPIOE_BASE + 0x00))
#define GPIOE_PUPDR    (*(volatile uint32_t *)(GPIOE_BASE + 0x0C))
#define GPIOE_IDR      (*(volatile uint32_t *)(GPIOE_BASE + 0x10))

#define TIM2_BASE      0x40000000UL
#define TIM2_CR1       (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_EGR       (*(volatile uint32_t *)(TIM2_BASE + 0x14))
#define TIM2_CNT       (*(volatile uint32_t *)(TIM2_BASE + 0x24))
#define TIM2_PSC       (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR       (*(volatile uint32_t *)(TIM2_BASE + 0x2C))

#define TIM4_BASE      0x40000800UL
#define TIM4_CR1       (*(volatile uint32_t *)(TIM4_BASE + 0x00))
#define TIM4_EGR       (*(volatile uint32_t *)(TIM4_BASE + 0x14))
#define TIM4_CCMR1     (*(volatile uint32_t *)(TIM4_BASE + 0x18))
#define TIM4_CCER      (*(volatile uint32_t *)(TIM4_BASE + 0x20))
#define TIM4_PSC       (*(volatile uint32_t *)(TIM4_BASE + 0x28))
#define TIM4_ARR       (*(volatile uint32_t *)(TIM4_BASE + 0x2C))
#define TIM4_CCR1      (*(volatile uint32_t *)(TIM4_BASE + 0x34))

#define SCB_CPACR      (*(volatile uint32_t *)0xE000ED88UL)

#define ESPERANDO  0
#define ENTRANDO   1
#define SALIENDO   2
#define LLENO      3

int estado = ESPERANDO;
int vehiculos = 0;
int capacidad = 3;
int disponibles = 3;
int entradaAnterior = 0;
int salidaAnterior = 0;
int pasoDetectado = 0;

void SystemInit(void)
{
    SCB_CPACR |= (0xFUL << 20);

    RCC_CR |= (1UL << 0);

    while((RCC_CR & (1UL << 1)) == 0)
    {
    }

    RCC_CFGR &= ~(3UL << 0);
    RCC_CFGR &= ~(0xFUL << 4);
    RCC_CFGR &= ~(7UL << 10);
}

void Timer2_Init(void)
{
    RCC_APB1ENR |= (1UL << 0);

    TIM2_PSC = 15;
    TIM2_ARR = 0xFFFFFFFF;
    TIM2_EGR = 1;
    TIM2_CR1 |= 1;
}

void delay_us(uint32_t tiempo)
{
    uint32_t inicio = TIM2_CNT;

    while((uint32_t)(TIM2_CNT - inicio) < tiempo)
    {
    }
}

void delay_ms(uint32_t tiempo)
{
    while(tiempo > 0)
    {
        delay_us(1000);
        tiempo--;
    }
}

void GPIO_Init(void)
{
    RCC_AHB1ENR |= (1UL << 0);
    RCC_AHB1ENR |= (1UL << 1);
    RCC_AHB1ENR |= (1UL << 3);
    RCC_AHB1ENR |= (1UL << 4);

    // PA0 TRIG
    GPIOA_MODER &= ~(3UL << 0);
    GPIOA_MODER |= (1UL << 0);

    // PA1 ECHO
    GPIOA_MODER &= ~(3UL << 2);

    // PE0 SENSOR ENTRADA
    GPIOE_MODER &= ~(3UL << 0);
    GPIOE_PUPDR &= ~(3UL << 0);
    GPIOE_PUPDR |= (1UL << 0);

    // PE1 SENSOR SALIDA
    GPIOE_MODER &= ~(3UL << 2);
    GPIOE_PUPDR &= ~(3UL << 2);
    GPIOE_PUPDR |= (1UL << 2);

    // LEDs PD12 - PD15
    GPIOD_MODER &= ~(3UL << 24);
    GPIOD_MODER |= (1UL << 24);

    GPIOD_MODER &= ~(3UL << 26);
    GPIOD_MODER |= (1UL << 26);

    GPIOD_MODER &= ~(3UL << 28);
    GPIOD_MODER |= (1UL << 28);

    GPIOD_MODER &= ~(3UL << 30);
    GPIOD_MODER |= (1UL << 30);
}

void Servo_Init(void)
{
    RCC_APB1ENR |= (1UL << 2);

    GPIOB_MODER &= ~(3UL << 12);
    GPIOB_MODER |= (2UL << 12);

    GPIOB_AFRL &= ~(0xFUL << 24);
    GPIOB_AFRL |= (2UL << 24);

    TIM4_PSC = 15;
    TIM4_ARR = 19999;

    TIM4_CCMR1 &= ~(7UL << 4);
    TIM4_CCMR1 |= (6UL << 4);
    TIM4_CCMR1 |= (1UL << 3);

    TIM4_CCER |= 1;

    TIM4_CCR1 = 1000;

    TIM4_EGR = 1;
    TIM4_CR1 |= 1;
}

void Servo_Abrir(void)
{
    TIM4_CCR1 = 2000;
}

void Servo_Cerrar(void)
{
    TIM4_CCR1 = 1000;
}

int LeerEntrada(void)
{
    if((GPIOE_IDR & (1UL << 0)) == 0)
        return 1;

    return 0;
}

int LeerSalida(void)
{
    if((GPIOE_IDR & (1UL << 1)) == 0)
        return 1;

    return 0;
}

uint32_t LeerDistancia(void)
{
    uint32_t inicio;
    uint32_t tiempo;

    GPIOA_ODR &= ~(1UL << 0);
    delay_us(2);

    GPIOA_ODR |= (1UL << 0);
    delay_us(10);
    GPIOA_ODR &= ~(1UL << 0);

    inicio = TIM2_CNT;

    while((GPIOA_IDR & (1UL << 1)) == 0)
    {
        if((uint32_t)(TIM2_CNT - inicio) > 30000)
            return 999;
    }

    inicio = TIM2_CNT;

    while((GPIOA_IDR & (1UL << 1)) != 0)
    {
        if((uint32_t)(TIM2_CNT - inicio) > 30000)
            return 999;
    }

    tiempo = (uint32_t)(TIM2_CNT - inicio);

    return tiempo / 58;
}

void ApagarLeds(void)
{
    GPIOD_ODR &= ~((1UL << 12) |
                   (1UL << 13) |
                   (1UL << 14) |
                   (1UL << 15));
}

void MostrarEstado(void)
{
    ApagarLeds();

    if(estado == ESPERANDO)
        GPIOD_ODR |= (1UL << 12);

    else if(estado == ENTRANDO)
        GPIOD_ODR |= (1UL << 15);

    else if(estado == SALIENDO)
        GPIOD_ODR |= (1UL << 13);

    else if(estado == LLENO)
        GPIOD_ODR |= (1UL << 14);
}

void MaquinaEstados(void)
{
    int entrada = LeerEntrada();
    int salida = LeerSalida();
    uint32_t distancia;

    switch(estado)
    {
        case ESPERANDO:

            if((entrada == 1) && (entradaAnterior == 0))
            {
                delay_ms(30);

                if(LeerEntrada() == 1)
                {
                    if(vehiculos < capacidad)
                    {
                        estado = ENTRANDO;
                        pasoDetectado = 0;

                        Servo_Abrir();
                        MostrarEstado();
                    }
                }
            }

            else if((salida == 1) && (salidaAnterior == 0))
            {
                delay_ms(30);

                if(LeerSalida() == 1)
                {
                    if(vehiculos > 0)
                    {
                        estado = SALIENDO;
                        pasoDetectado = 0;

                        Servo_Abrir();
                        MostrarEstado();
                    }
                }
            }

            break;

        case ENTRANDO:

            distancia = LeerDistancia();

            if((distancia > 2) && (distancia < 15))
                pasoDetectado = 1;

            if((pasoDetectado == 1) && (distancia > 25))
            {
                vehiculos++;

                disponibles = capacidad - vehiculos;

                Servo_Cerrar();

                pasoDetectado = 0;

                if(vehiculos >= capacidad)
                    estado = LLENO;
                else
                    estado = ESPERANDO;

                MostrarEstado();

                delay_ms(500);
            }

            delay_ms(50);

            break;

        case SALIENDO:

            distancia = LeerDistancia();

            if((distancia > 2) && (distancia < 15))
                pasoDetectado = 1;

            if((pasoDetectado == 1) && (distancia > 25))
            {
                if(vehiculos > 0)
                    vehiculos--;

                disponibles = capacidad - vehiculos;

                Servo_Cerrar();

                pasoDetectado = 0;
                estado = ESPERANDO;

                MostrarEstado();

                delay_ms(500);
            }

            delay_ms(50);

            break;

        case LLENO:

            if((salida == 1) && (salidaAnterior == 0))
            {
                delay_ms(30);

                if(LeerSalida() == 1)
                {
                    if(vehiculos > 0)
                    {
                        estado = SALIENDO;
                        pasoDetectado = 0;

                        Servo_Abrir();
                        MostrarEstado();
                    }
                }
            }

            break;

        default:

            estado = ESPERANDO;

            Servo_Cerrar();
            MostrarEstado();

            break;
    }

    entradaAnterior = entrada;
    salidaAnterior = salida;
}

int main(void)
{
    Timer2_Init();
    GPIO_Init();
    Servo_Init();

    estado = ESPERANDO;
    vehiculos = 0;
    disponibles = capacidad;

    Servo_Cerrar();

    entradaAnterior = LeerEntrada();
    salidaAnterior = LeerSalida();

    MostrarEstado();

    while(1)
    {
        MaquinaEstados();
    }

    return 0;
}