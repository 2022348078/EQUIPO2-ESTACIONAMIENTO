#ifndef ESTACIONAMIENTO_H
#define ESTACIONAMIENTO_H

#include <stdint.h>


/* =========================================================
   ESTADOS
   ========================================================= */

typedef enum
{
    ESPERANDO = 0,
    ABRIENDO,
    VEHICULO_PASANDO,
    CERRANDO,
    LLENO

} EstadoEstacionamiento;


/* =========================================================
   DIRECCION DEL VEHICULO
   ========================================================= */

typedef enum
{
    NINGUNA = 0,
    ENTRADA,
    SALIDA

} DireccionVehiculo;


/* =========================================================
   VARIABLES
   ========================================================= */

extern EstadoEstacionamiento estadoActual;

extern int vehiculosDentro;
extern int espaciosDisponibles;
extern int espaciosTotales;


/* =========================================================
   FUNCIONES
   ========================================================= */

void Estacionamiento_Init(int espacios);

void Estacionamiento_Actualizar(void);

#endif
