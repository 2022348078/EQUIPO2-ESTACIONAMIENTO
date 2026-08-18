/*
 * ==============================================================================
 * PROYECTO: EQUIPO2-ESTACIONAMIENTO
 * ACTIVIDAD 1: Máquina de Estados + Sensores (Ultrasónico + Fotoresistencia)
 * DESCRIPCIÓN: Controla el acceso con el sensor HC-SR04, detecta Día/Noche con
 *              la fotoresistencia en PA2, y maneja los LEDs de la placa:
 *              - Verde (PD12) = Libre
 *              - Rojo  (PD14) = Lleno
 *              - Azul  (PD15) = Parpadea cuando un auto está cruzando
 * ==============================================================================
 */

#include "maquina_estados.h"

// Direcciones de registros para control directo de hardware (STM32F407)
#define PERIPH_BASE           ((uint32_t)0x40000000)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800)

// Registros Puerto A (Sensores)
#define RCC_AHB1ENR           (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define GPIOA_MODER           (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_PUPDR           (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
#define GPIOA_ODR             (*(volatile uint32_t *)(GPIOA_BASE + 0x14))
#define GPIOA_IDR             (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

// Registros Puerto D (LEDs de la placa)
#define GPIOD_MODER           (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_ODR             (*(volatile uint32_t *)(GPIOD_BASE + 0x14))

// Parámetros de configuración del estacionamiento
#define LIMITE_MAX_AUTOS      10
#define DISTANCIA_DETECCION   10  // Distancia en centímetros para detectar carro

// Variables de estado globales del módulo
static EstadoSistema estado_actual = ESTADO_LIBRE;
static int contador_autos = 0;
static int es_de_noche = 0;

// Función interna para generar retardos de tiempo
void Delay_us(uint32_t us) {
    volatile uint32_t count = us * 3;
    while(count--) {
        __asm("nop");
    }
}

/*
 * FUNCIÓN: HCSR04_LeerDistancia
 * DESCRIPCIÓN: Mide la distancia del sensor ultrasónico en centímetros.
 */
uint32_t HCSR04_LeerDistancia(uint32_t trig_pin, uint32_t echo_pin) {
    uint32_t local_time = 0;

    GPIOA_ODR &= ~(1 << trig_pin);
    Delay_us(2);

    GPIOA_ODR |= (1 << trig_pin);
    Delay_us(10);
    GPIOA_ODR &= ~(1 << trig_pin);

    uint32_t timeout = 10000;
    while (!(GPIOA_IDR & (1 << echo_pin)) && timeout--) { __asm("nop"); }
    if(timeout == 0) return 999;

    while ((GPIOA_IDR & (1 << echo_pin))) {
        local_time++;
        Delay_us(1);
        if(local_time > 20000) break;
    }

    return (local_time / 14);
}

/*
 * FUNCIÓN: FSM_Init
 * DESCRIPCIÓN: Configura los pines de los sensores y de los LEDs de la placa.
 *              - PA0 (Trig) como Salida, PA1 (Echo) como Entrada.
 *              - PA2 (Fotoresistencia) como Entrada con Pull-Up.
 *              - PD12 (Verde), PD14 (Rojo) y PD15 (Azul) como Salidas.
 */
void FSM_Init(void) {
    // 1. Activar los relojes para el Puerto A y Puerto D
    RCC_AHB1ENR |= (1 << 0) | (1 << 3); // Bit 0 = GPIOA, Bit 3 = GPIOD

    // 2. Configurar pines del Puerto A: PA0=Salida(01), PA1=Entrada(00), PA2=Entrada(00)
    GPIOA_MODER &= ~(0x0000003F);
    GPIOA_MODER |=  (0x00000001); // PA0 como salida

    // 3. Configurar resistencia Pull-Up interna en PA2 para la Fotoresistencia
    GPIOA_PUPDR &= ~(0x00000030); // Limpia PA2
    GPIOA_PUPDR |=  (0x00000010); // Activa Pull-Up en PA2 (01)

    // 4. Configurar LEDs internos de la placa (PD12, PD14 y PD15) como salidas digitales (01)
    GPIOD_MODER &= ~((3 << 24) | (3 << 28) | (3 << 30)); // Limpia los modos correspondientes
    GPIOD_MODER |=  ((1 << 24) | (1 << 28) | (1 << 30)); // Configura PD12, PD14 y PD15 como salidas
}

/*
 * FUNCIÓN: FSM_Procesar
 * DESCRIPCIÓN: Lógica principal. Mide sensores y actualiza los indicadores luminosos.
 */
void FSM_Procesar(void) {
    // A. Leer la fotoresistencia en PA2 (1 = Oscuridad/Noche, 0 = Claridad/Día)
    es_de_noche = (GPIOA_IDR & (1 << 2)) ? 1 : 0;

    // B. Leer la distancia del sensor ultrasónico
    uint32_t distancia = HCSR04_LeerDistancia(0, 1);
    int vehiculo_detectado = (distancia < DISTANCIA_DETECCION) ? 1 : 0;

    // C. Ejecución de la Máquina de Estados
    switch (estado_actual) {

        case ESTADO_LIBRE:
            // Estacionamiento disponible: LED Verde encendido, Rojo y Azul apagados
            GPIOD_ODR |=  (1 << 12);
            GPIOD_ODR &= ~(1 << 14);
            GPIOD_ODR &= ~(1 << 15);

            if (vehiculo_detectado && (contador_autos < LIMITE_MAX_AUTOS)) {
                estado_actual = ESTADO_ENTRANDO;
            }
            break;

        case ESTADO_ENTRANDO:
            // Auto cruzando: Apagar Verde e intercalar (conmutar) el estado del LED Azul para que parpadee
            GPIOD_ODR &= ~(1 << 12);
            GPIOD_ODR ^=  (1 << 15); // El operador ^= cambia el LED de encendido a apagado en cada ciclo

            if (!vehiculo_detectado) { // El auto terminó de pasar
                GPIOD_ODR &= ~(1 << 15); // Asegura apagar el LED azul al salir del estado
                contador_autos++;
                if (contador_autos >= LIMITE_MAX_AUTOS) {
                    estado_actual = ESTADO_LLENO;
                } else {
                    estado_actual = ESTADO_LIBRE;
                }
            }
            break;

        case ESTADO_SALIENDO:
            // Auto saliendo: Parpadear LED Azul
            GPIOD_ODR ^= (1 << 15);

            if (!vehiculo_detectado) {
                GPIOD_ODR &= ~(1 << 15);
                if (contador_autos > 0) contador_autos--;
                estado_actual = ESTADO_LIBRE;
            }
            break;

        case ESTADO_LLENO:
            // Estacionamiento lleno: LED Rojo encendido, Verde y Azul apagados
            GPIOD_ODR &= ~(1 << 12);
            GPIOD_ODR |=  (1 << 14);
            GPIOD_ODR &= ~(1 << 15);

            // Si detecta un carro queriendo salir en el sensor, pasamos a estado saliendo
            if (vehiculo_detectado) {
                // Nota: En una instalación real usarías un segundo sensor,
                // aquí usamos el mismo para simular la acción de salida.
                estado_actual = ESTADO_SALIENDO;
            }
            break;
    }

    // Pausa de estabilidad (50ms). Controla también la velocidad del parpadeo del LED Azul
    Delay_us(50000);
}

EstadoSistema FSM_ObtenerEstadoActual(void) {
    return estado_actual;
}

int FSM_ObtenerContadorAutos(void) {
    return contador_autos;
}

int FSM_ObtenerLuzAmbiente(void) {
    return es_de_noche;
}
