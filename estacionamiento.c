#include "estacionamiento.h"
#include "uart_driver.h"
#include <stdio.h>

// Variables globales
int estadoActual = ESPERANDO;
int vehiculosDentro = 0;
int espaciosDisponibles = 0;
int espaciosTotales = 0;
int sensorEntrada = 0;
int sensorSalida = 0;

// Inicializa el sistema
void iniciarEstacionamiento(int totalEspacios)
{
    estadoActual = ESPERANDO;
    vehiculosDentro = 0;
    espaciosTotales = totalEspacios;
    espaciosDisponibles = totalEspacios;

    enviarEstadoUART();
}

// Maquina de estados: se llama en el while(1) del main
void actualizarEstado(void)
{
    if (estadoActual == ESPERANDO)
    {
        if (sensorEntrada == 1)
        {
            if (espaciosDisponibles > 0)
            {
                estadoActual = ABRIENDO;
            }
            else
            {
                estadoActual = LLENO;
            }
            enviarEstadoUART();
        }
    }
    else if (estadoActual == ABRIENDO)
    {
        // Aqui el compañero de PWM mueve el servo a 90 grados
        estadoActual = VEHICULO_PASANDO;
        enviarEstadoUART();
    }
    else if (estadoActual == VEHICULO_PASANDO)
    {
        if (sensorEntrada == 0) // el carro ya paso el sensor
        {
            vehiculosDentro = vehiculosDentro + 1;
            espaciosDisponibles = espaciosDisponibles - 1;
            estadoActual = CERRANDO;
            enviarEstadoUART();
        }
    }
    else if (estadoActual == CERRANDO)
    {
        // Aqui el compañero de PWM mueve el servo a 0 grados
        estadoActual = ESPERANDO;
        enviarEstadoUART();
    }
    else if (estadoActual == LLENO)
    {
        if (sensorSalida == 1)
        {
            vehiculosDentro = vehiculosDentro - 1;
            espaciosDisponibles = espaciosDisponibles + 1;
            estadoActual = ESPERANDO;
            enviarEstadoUART();
        }
    }
}

// Se llama cuando llega un dato por UART (comando A, C o E)
void procesarComandoUART(char comando)
{
    if (comando == 'A')
    {
        if (estadoActual == ESPERANDO && espaciosDisponibles > 0)
        {
            estadoActual = ABRIENDO;
        }
    }
    else if (comando == 'C')
    {
        if (estadoActual == VEHICULO_PASANDO)
        {
            estadoActual = CERRANDO;
        }
    }
    else if (comando == 'E')
    {
        enviarEstadoUART();
        return;
    }

    enviarEstadoUART();
}

// Envia el estado actual por UART
void enviarEstadoUART(void)
{
    char mensaje[80];
    char textoEstado[15];

    if (estadoActual == ESPERANDO)
        sprintf(textoEstado, "ESPERANDO");
    else if (estadoActual == ABRIENDO)
        sprintf(textoEstado, "ABRIENDO");
    else if (estadoActual == VEHICULO_PASANDO)
        sprintf(textoEstado, "PASANDO");
    else if (estadoActual == CERRANDO)
        sprintf(textoEstado, "CERRANDO");
    else if (estadoActual == LLENO)
        sprintf(textoEstado, "LLENO");

    sprintf(mensaje, "Vehiculos: %d\r\nDisponibles: %d\r\nBarrera: %s\r\n",
            vehiculosDentro, espaciosDisponibles, textoEstado);

    UART_EnviarString(mensaje);
}
