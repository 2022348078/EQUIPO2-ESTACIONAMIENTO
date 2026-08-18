#ifndef ESTACIONAMIENTO_H
#define ESTACIONAMIENTO_H

// Estados posibles del sistema
#define ESPERANDO 0
#define ABRIENDO 1
#define VEHICULO_PASANDO 2
#define CERRANDO 3
#define LLENO 4

// Variables globales del sistema (compartidas con main.c)
extern int estadoActual;
extern int vehiculosDentro;
extern int espaciosDisponibles;
extern int espaciosTotales;
extern int sensorEntrada;
extern int sensorSalida;

// Funciones de la maquina de estados
void iniciarEstacionamiento(int totalEspacios);
void actualizarEstado(void);

// Funciones de UART
void procesarComandoUART(char comando);
void enviarEstadoUART(void);

#endif
