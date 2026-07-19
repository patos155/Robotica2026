// =============================================================
// Nombre:                          Pines y valores importantes
// Descripcion:                     Pines que se usan en el Arduino y valores que pueden llegar a modificarse
// Restricciones importantes:       Modulo que maneja todos los pines del Arduino y valores de la velocidad de motores y limites de distancia de sensores ultrasonicos
// =============================================================

#pragma once
#include <Arduino.h>

constexpr int PINS_RC[6]        = {25,29,27,31,33,35};
constexpr int pulseInDelay      = 30000;

//pines para control de velocidad de arranque 
constexpr int PIN_PWM_LEFT      = 6;
constexpr int PIN_PWM_RIGHT     = 7;

// definicion de salidas para control de los relevadores
constexpr int PIN_RELAY_LEFT_1  = 10;
constexpr int PIN_RELAY_LEFT_2  = 9;
constexpr int PIN_RELAY_RIGHT_1 = 15;
constexpr int PIN_RELAY_RIGHT_2 = 16;

// Sensores ultrasonicos
// conexiones de triger y echo Izquierdo trasero 
constexpr int PIN_LEFT_REAR_TRIGGER     = 41;           
constexpr int PIN_LEFT_REAR_ECHO        = 42;

// conexiones de triger y echo Izquierdo delantero
constexpr int PIN_LEFT_FRONT_TRIGGER    = 5;
constexpr int PIN_LEFT_FRONT_ECHO       = 4;

// conexiones de triger y echo delantero izquierdo
constexpr int PIN_FRONT_LEFT_TRIGGER    = 19;
constexpr int PIN_FRONT_LEFT_ECHO       = 20;

// conexiones de triger y echo delantero derecho
constexpr int PIN_FRONT_RIGHT_TRIGGER   = 8;
constexpr int PIN_FRONT_RIGHT_ECHO      = 40;

// conexiones de triger y echo derecho delantero
constexpr int PIN_RIGHT_FRONT_TRIGGER   = 21;
constexpr int PIN_RIGHT_FRONT_ECHO      = 22;

// conexiones de triger y echo derecho trasero
constexpr int PIN_RIGHT_REAR_TRIGGER    = 23;
constexpr int PIN_RIGHT_REAR_ECHO       = 24;

// Variables de distancia minima a paredes con ultrasonicos
constexpr int LATERAL_DISTANCE      = 25;         // Distancia lateral
constexpr int FRONT_DISTANCE        = 25;         // Distancia delantera

// Potencias para motores 
constexpr int SPEED_LEFT_FRONT      = 150;
constexpr int SPEED_LEFT_TURN       = 200;
constexpr int SPEED_RIGHT_TURN      = SPEED_LEFT_TURN;
constexpr int SPEED_RIGHT_FRONT     = SPEED_LEFT_FRONT;

constexpr uint8_t  MOT_FORWARD      = HIGH;
constexpr uint8_t  MOT_STOP         = LOW;