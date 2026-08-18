#include <stdint.h>

#include "estacionamiento.h"
#include "stm32f407_reg.h"


/* =========================================================
   VARIABLES GLOBALES
   ========================================================= */

EstadoEstacionamiento estadoActual = ESPERANDO;

int vehiculosDentro = 0;

int espaciosDisponibles = 0;

int espaciosTotales = 0;


/* Direccion actual */

static DireccionVehiculo direccion = NINGUNA;


/* Estado anterior de los sensores */

static uint8_t sensorEntradaAnterior = 0;

static uint8_t sensorSalidaAnterior = 0;


/* =========================================================
   DELAY
   Reloj utilizado: 16 MHz
   ========================================================= */

static void delay_ms(uint32_t tiempo)
{
    uint32_t i;


    /*
       16 MHz

       16000 ciclos = 1 ms
    */

    SYSTICK_LOAD = 16000UL - 1UL;

    SYSTICK_VAL = 0;


    /*
       Bit 0 = ENABLE
       Bit 2 = CLKSOURCE

       101 = 5
    */

    SYSTICK_CTRL = 5;


    for(i = 0; i < tiempo; i++)
    {
        while((SYSTICK_CTRL & (1UL << 16)) == 0)
        {
            /* Esperar */
        }
    }


    SYSTICK_CTRL = 0;
}


/* =========================================================
   SENSOR ENTRADA PE0

   Sensor considerado activo en bajo:

   0 = detecta objeto
   1 = libre
   ========================================================= */

static uint8_t LeerSensorEntrada(void)
{
    if((GPIOE_IDR & (1UL << 0)) == 0)
    {
        return 1;
    }

    return 0;
}


/* =========================================================
   SENSOR SALIDA PE1
   ========================================================= */

static uint8_t LeerSensorSalida(void)
{
    if((GPIOE_IDR & (1UL << 1)) == 0)
    {
        return 1;
    }

    return 0;
}


/* =========================================================
   APAGAR LEDS
   ========================================================= */

static void ApagarLeds(void)
{
    GPIOD_ODR &= ~(
                    (1UL << 12) |
                    (1UL << 13) |
                    (1UL << 14) |
                    (1UL << 15)
                   );
}


/* =========================================================
   MOSTRAR ESTADO

   PD12 = Verde
   PD13 = Naranja
   PD14 = Rojo
   PD15 = Azul
   ========================================================= */

static void MostrarEstado(void)
{
    ApagarLeds();


    switch(estadoActual)
    {
        case ESPERANDO:

            /* Verde */

            GPIOD_ODR |= (1UL << 12);

            break;


        case ABRIENDO:

            /* Naranja */

            GPIOD_ODR |= (1UL << 13);

            break;


        case VEHICULO_PASANDO:

            /* Rojo */

            GPIOD_ODR |= (1UL << 14);

            break;


        case CERRANDO:

            /* Azul */

            GPIOD_ODR |= (1UL << 15);

            break;


        case LLENO:

            /* Rojo */

            GPIOD_ODR |= (1UL << 14);

            break;


        default:

            break;
    }
}


/* =========================================================
   SERVO CERRADO
   ========================================================= */

static void Servo_Cerrar(void)
{
    /*
       1000 us aproximadamente.
    */

    TIM4_CCR1 = 1000;
}


/* =========================================================
   SERVO ABIERTO
   ========================================================= */

static void Servo_Abrir(void)
{
    /*
       2000 us aproximadamente.
    */

    TIM4_CCR1 = 2000;
}


/* =========================================================
   GPIO INIT
   ========================================================= */

static void GPIO_Init(void)
{
    /* =====================================================
       HABILITAR GPIO

       GPIOB = bit 1
       GPIOD = bit 3
       GPIOE = bit 4
       ===================================================== */

    RCC_AHB1ENR |= (1UL << 1);

    RCC_AHB1ENR |= (1UL << 3);

    RCC_AHB1ENR |= (1UL << 4);


    /* Pequeña lectura para asegurar activacion */

    (void)RCC_AHB1ENR;


    /* =====================================================
       PE0 SENSOR ENTRADA
       00 = INPUT
       ===================================================== */

    GPIOE_MODER &= ~(3UL << 0);


    /* Pull-up */

    GPIOE_PUPDR &= ~(3UL << 0);

    GPIOE_PUPDR |= (1UL << 0);


    /* =====================================================
       PE1 SENSOR SALIDA
       ===================================================== */

    GPIOE_MODER &= ~(3UL << 2);


    /* Pull-up */

    GPIOE_PUPDR &= ~(3UL << 2);

    GPIOE_PUPDR |= (1UL << 2);


    /* =====================================================
       PD12 LED VERDE
       01 = OUTPUT
       ===================================================== */

    GPIOD_MODER &= ~(3UL << 24);

    GPIOD_MODER |= (1UL << 24);


    /* PD13 */

    GPIOD_MODER &= ~(3UL << 26);

    GPIOD_MODER |= (1UL << 26);


    /* PD14 */

    GPIOD_MODER &= ~(3UL << 28);

    GPIOD_MODER |= (1UL << 28);


    /* PD15 */

    GPIOD_MODER &= ~(3UL << 30);

    GPIOD_MODER |= (1UL << 30);


    ApagarLeds();
}


/* =========================================================
   PWM INIT

   PB6 = TIM4 CH1
   ========================================================= */

static void PWM_Init(void)
{
    /* =====================================================
       HABILITAR TIM4

       APB1ENR bit 2
       ===================================================== */

    RCC_APB1ENR |= (1UL << 2);

    (void)RCC_APB1ENR;


    /* =====================================================
       PB6 COMO FUNCION ALTERNATIVA

       MODER6 = 10
       ===================================================== */

    GPIOB_MODER &= ~(3UL << 12);

    GPIOB_MODER |= (2UL << 12);


    /* =====================================================
       PB6 AF2 = TIM4_CH1
       ===================================================== */

    GPIOB_AFRL &= ~(0xFUL << 24);

    GPIOB_AFRL |= (2UL << 24);


    /* =====================================================
       PWM 50 Hz

       HSI = 16 MHz

       PSC = 15

       16 MHz / 16 = 1 MHz
       ===================================================== */

    TIM4_PSC = 15;


    /*
       ARR = 19999

       1 MHz / 20000 = 50 Hz
    */

    TIM4_ARR = 19999;


    /* =====================================================
       PWM MODE 1

       OC1M = 110
       ===================================================== */

    TIM4_CCMR1 &= ~(7UL << 4);

    TIM4_CCMR1 |= (6UL << 4);


    /* Preload */

    TIM4_CCMR1 |= (1UL << 3);


    /* Habilitar canal 1 */

    TIM4_CCER |= (1UL << 0);


    /* Posicion inicial */

    TIM4_CCR1 = 1000;


    /* Generar update */

    TIM4_EGR |= (1UL << 0);


    /* Iniciar TIM4 */

    TIM4_CR1 |= (1UL << 0);
}


/* =========================================================
   INICIALIZAR ESTACIONAMIENTO
   ========================================================= */

void Estacionamiento_Init(int espacios)
{
    GPIO_Init();

    PWM_Init();


    espaciosTotales = espacios;

    espaciosDisponibles = espacios;

    vehiculosDentro = 0;


    estadoActual = ESPERANDO;

    direccion = NINGUNA;


    Servo_Cerrar();


    sensorEntradaAnterior = LeerSensorEntrada();

    sensorSalidaAnterior = LeerSensorSalida();


    MostrarEstado();
}


/* =========================================================
   MAQUINA DE ESTADOS
   ========================================================= */

void Estacionamiento_Actualizar(void)
{
    uint8_t sensorEntrada;

    uint8_t sensorSalida;


    sensorEntrada = LeerSensorEntrada();

    sensorSalida = LeerSensorSalida();


    switch(estadoActual)
    {

        /* =================================================
           ESPERANDO
           ================================================= */

        case ESPERANDO:


            /*
               Detectar vehiculo entrando.
            */

            if((sensorEntrada == 1) &&
               (sensorEntradaAnterior == 0))
            {
                delay_ms(20);


                if(LeerSensorEntrada() == 1)
                {
                    if(espaciosDisponibles > 0)
                    {
                        direccion = ENTRADA;

                        estadoActual = ABRIENDO;
                    }

                    else
                    {
                        estadoActual = LLENO;
                    }
                }
            }


            /*
               Detectar vehiculo saliendo.
            */

            else if((sensorSalida == 1) &&
                    (sensorSalidaAnterior == 0))
            {
                delay_ms(20);


                if(LeerSensorSalida() == 1)
                {
                    if(vehiculosDentro > 0)
                    {
                        direccion = SALIDA;

                        estadoActual = ABRIENDO;
                    }
                }
            }


            break;



        /* =================================================
           ABRIENDO
           ================================================= */

        case ABRIENDO:


            MostrarEstado();


            Servo_Abrir();


            /*
               Tiempo para que se mueva
               el SG90.
            */

            delay_ms(700);


            estadoActual = VEHICULO_PASANDO;


            break;



        /* =================================================
           VEHICULO PASANDO
           ================================================= */

        case VEHICULO_PASANDO:


            /*
               VEHICULO ENTRANDO
            */

            if(direccion == ENTRADA)
            {
                /*
                   Cuando PE0 deja de detectar,
                   el vehiculo termino de pasar.
                */

                if(sensorEntrada == 0)
                {
                    if(vehiculosDentro < espaciosTotales)
                    {
                        vehiculosDentro++;

                        espaciosDisponibles--;
                    }


                    estadoActual = CERRANDO;
                }
            }


            /*
               VEHICULO SALIENDO
            */

            else if(direccion == SALIDA)
            {
                if(sensorSalida == 0)
                {
                    if(vehiculosDentro > 0)
                    {
                        vehiculosDentro--;

                        espaciosDisponibles++;
                    }


                    estadoActual = CERRANDO;
                }
            }


            break;



        /* =================================================
           CERRANDO
           ================================================= */

        case CERRANDO:


            MostrarEstado();


            delay_ms(500);


            Servo_Cerrar();


            delay_ms(700);


            direccion = NINGUNA;


            /*
               Revisar si se lleno.
            */

            if(espaciosDisponibles <= 0)
            {
                espaciosDisponibles = 0;

                estadoActual = LLENO;
            }

            else
            {
                estadoActual = ESPERANDO;
            }


            break;



        /* =================================================
           LLENO
           ================================================= */

        case LLENO:


            /*
               Si esta lleno, solamente
               permitimos salidas.
            */

            if((sensorSalida == 1) &&
               (sensorSalidaAnterior == 0))
            {
                delay_ms(20);


                if(LeerSensorSalida() == 1)
                {
                    if(vehiculosDentro > 0)
                    {
                        direccion = SALIDA;

                        estadoActual = ABRIENDO;
                    }
                }
            }


            break;



        /* =================================================
           DEFAULT
           ================================================= */

        default:


            estadoActual = ESPERANDO;

            direccion = NINGUNA;

            Servo_Cerrar();


            break;
    }


    MostrarEstado();


    sensorEntradaAnterior = sensorEntrada;

    sensorSalidaAnterior = sensorSalida;
}
