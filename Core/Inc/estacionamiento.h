#ifndef ESTACIONAMIENTO_H
#define ESTACIONAMIENTO_H

#include <stdint.h>

/* Estados de la maquina de estados */
typedef enum
{
    ESPERANDO = 0,
    ABRIENDO,
    VEHICULO_PASANDO,
    CERRANDO,
    ESTACIONAMIENTO_LLENO

} EstadoEstacionamiento;


/* Informacion principal del estacionamiento */
typedef struct
{
    EstadoEstacionamiento estado_actual;

    uint8_t vehiculos;
    uint8_t capacidad_maxima;

    uint8_t barrera_abierta;
    uint8_t estacionamiento_lleno;

} Estacionamiento;


/* Inicializa el sistema */
void Estacionamiento_Init(
        Estacionamiento *sistema,
        uint8_t capacidad);


/* Ejecuta la maquina de estados */
void Estacionamiento_Actualizar(
        Estacionamiento *sistema,
        uint8_t sensor_entrada,
        uint8_t sensor_salida);


/* Obtiene los lugares disponibles */
uint8_t Estacionamiento_Disponibles(
        Estacionamiento *sistema);


/* Obtiene el estado actual */
EstadoEstacionamiento Estacionamiento_ObtenerEstado(
        Estacionamiento *sistema);


#endif
