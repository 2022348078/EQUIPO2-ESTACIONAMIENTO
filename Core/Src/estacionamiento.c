#include "estacionamiento.h"


void Estacionamiento_Init(
        Estacionamiento *sistema,
        uint8_t capacidad)
{
    sistema->estado_actual = ESPERANDO;
    sistema->vehiculos = 0;
    sistema->capacidad_maxima = capacidad;
    sistema->barrera_abierta = 0;
    sistema->estacionamiento_lleno = 0;
}


void Estacionamiento_Actualizar(
        Estacionamiento *sistema,
        uint8_t sensor_entrada,
        uint8_t sensor_salida)
{
    switch (sistema->estado_actual)
    {
        case ESPERANDO:

            sistema->barrera_abierta = 0;

            if (sistema->vehiculos >= sistema->capacidad_maxima)
            {
                sistema->estacionamiento_lleno = 1;
                sistema->estado_actual = ESTACIONAMIENTO_LLENO;
            }
            else if (sensor_entrada == 1)
            {
                sistema->estacionamiento_lleno = 0;
                sistema->estado_actual = ABRIENDO;
            }
            else if (sensor_salida == 1)
            {
                if (sistema->vehiculos > 0)
                {
                    sistema->vehiculos--;
                }
            }

            break;


        case ABRIENDO:

            sistema->barrera_abierta = 1;
            sistema->estado_actual = VEHICULO_PASANDO;

            break;


        case VEHICULO_PASANDO:

            if (sensor_entrada == 0)
            {
                if (sistema->vehiculos < sistema->capacidad_maxima)
                {
                    sistema->vehiculos++;
                }

                sistema->estado_actual = CERRANDO;
            }

            break;


        case CERRANDO:

            sistema->barrera_abierta = 0;

            if (sistema->vehiculos >= sistema->capacidad_maxima)
            {
                sistema->estacionamiento_lleno = 1;
                sistema->estado_actual = ESTACIONAMIENTO_LLENO;
            }
            else
            {
                sistema->estado_actual = ESPERANDO;
            }

            break;


        case ESTACIONAMIENTO_LLENO:

            sistema->barrera_abierta = 0;
            sistema->estacionamiento_lleno = 1;

            if (sensor_salida == 1)
            {
                if (sistema->vehiculos > 0)
                {
                    sistema->vehiculos--;
                }

                sistema->estacionamiento_lleno = 0;
                sistema->estado_actual = ESPERANDO;
            }

            break;


        default:

            sistema->estado_actual = ESPERANDO;
            sistema->barrera_abierta = 0;
            sistema->estacionamiento_lleno = 0;

            break;
    }
}


uint8_t Estacionamiento_Disponibles(
        Estacionamiento *sistema)
{
    if (sistema->vehiculos >= sistema->capacidad_maxima)
    {
        return 0;
    }

    return sistema->capacidad_maxima - sistema->vehiculos;
}


EstadoEstacionamiento Estacionamiento_ObtenerEstado(
        Estacionamiento *sistema)
{
    return sistema->estado_actual;
}
