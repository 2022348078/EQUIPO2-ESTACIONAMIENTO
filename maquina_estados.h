/*
 * ==============================================================================
 * PROYECTO: EQUIPO2-ESTACIONAMIENTO
 * CABECERA DE INTERFAZ: maquina_estados.h
 * ==============================================================================
 */

#ifndef MAQUINA_ESTADOS_H_
#define MAQUINA_ESTADOS_H_

#include <stdint.h>

typedef enum {
    ESTADO_LIBRE = 0,
    ESTADO_ENTRANDO,
    ESTADO_SALIENDO,
    ESTADO_LLENO
} EstadoSistema;

// Prototipos de función públicos
void FSM_Init(void);
void FSM_Procesar(void);
EstadoSistema FSM_ObtenerEstadoActual(void);
int FSM_ObtenerContadorAutos(void);
int FSM_ObtenerLuzAmbiente(void); // 1 si es de noche, 0 si es de día

#endif /* MAQUINA_ESTADOS_H_ */
