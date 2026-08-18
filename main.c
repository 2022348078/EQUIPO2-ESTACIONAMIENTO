#include <stdint.h>

#include "stm32f407_reg.h"
#include "estacionamiento.h"


/* =========================================================
   SYSTEM INIT

   Esta funcion es llamada por algunos archivos Startup
   antes de entrar al main.

   Mantendremos el reloj inicial HSI de 16 MHz.
   ========================================================= */

void SystemInit(void)
{
    /*
       Habilitar FPU:
       CP10 y CP11 con acceso completo.
    */

    SCB_CPACR |= (0xFUL << 20);

    __asm volatile ("dsb");
    __asm volatile ("isb");
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    /*
       Estacionamiento con capacidad
       para 3 vehiculos.
    */

    Estacionamiento_Init(3);


    while(1)
    {
        Estacionamiento_Actualizar();
    }


    return 0;
}
