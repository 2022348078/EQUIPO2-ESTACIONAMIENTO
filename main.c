/*
 * ==============================================================================
 * PROYECTO: EQUIPO2-ESTACIONAMIENTO
 * ARCHIVO PRINCIPAL: main.c
 * DESCRIPCIÓN: Punto de entrada del sistema. Coordina la inicialización
 *              de los periféricos y ejecuta el ciclo infinito donde opera
 *              la máquina de estados del Integrante 1.
 * ==============================================================================
 */

#include <stdint.h>
#include "maquina_estados.h"

// Función para inicializar el reloj básico del sistema (System Clock)
void SystemClock_Config(void) {
    // Se deja vacía a propósito para usar el reloj interno por defecto (16 MHz)
    // Esto evita depender de archivos del chip ausentes en el repositorio vacío
}

/*
 * FUNCIÓN PRINCIPAL DEL SISTEMA
 */
int main(void) {
    // 1. Configurar el reloj del sistema básico
    SystemClock_Config();

    // 2. Inicializar la Máquina de Estados y los sensores del Integrante 1 (PA0 y PA1)
    FSM_Init();

    /*
     * CICLO INFINITO PRINCIPAL
     * Aquí se ejecuta de forma constante la lectura de hardware y lógica
     */
    while (1) {

        // 3. Procesar sensores y actualizar estados continuamente
        FSM_Procesar();

        /*
         * NOTA PARA EL EQUIPO:
         * El Integrante 2 (PWM), Integrante 3 (UART) e Integrante 4 (LEDs)
         * deben mandar a llamar sus funciones aquí abajo, utilizando las funciones:
         * - FSM_ObtenerEstadoActual()
         * - FSM_ObtenerContadorAutos()
         */
    }

    return 0;
}
