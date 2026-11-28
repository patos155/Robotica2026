// =============================================================
// Nombre:                          Pines y valores importantes
// Descripcion:                     Pines que se usan en el Arduino y valores que pueden llegar a modificarse
// Restricciones importantes:       Modulo que maneja todos los pines del Arduino y valores de la velocidad de motores y limites de distancia de sensores ultrasonicos
// =============================================================

#pragma once
#include <Arduino.h>
//#define DEBUG_MODE 

constexpr long SERIAL_BAUD_RATE             = 9600;
constexpr unsigned long SENSOR_TELEMETRY_MS = 200;

constexpr int PINS_RC[6]        = {25,29,27,31,33,35};
constexpr unsigned long PULSE_IN_TIMEOUT_US = 30000;      // tope de pulseIn() en RC y ultrasonicos
// Lecturas seguidas sin pulso en el canal del switch (ch4) para dar el control por perdido.
// Evita que un pulso perdido aislado detenga el robot (Python legacy manda "F" una sola vez).
constexpr uint8_t RC_LOST_READS = 3;

//pines para control de velocidad de arranque 
constexpr int PIN_PWM_LEFT      = 6;
constexpr int PIN_PWM_RIGHT     = 7;

// definicion de salidas para control de los relevadores
constexpr int PIN_RELAY_LEFT_1  = 10;
constexpr int PIN_RELAY_LEFT_2  = 9;
constexpr int PIN_RELAY_RIGHT_1 = 15;
constexpr int PIN_RELAY_RIGHT_2 = 16;
// Niveles para energizar o soltar un rele (no son direcciones)
constexpr uint8_t RELAY_ON      = HIGH;
constexpr uint8_t RELAY_OFF     = LOW;
// Pausa tras conmutar reles con el PWM en 0, antes de dar potencia, para que el contacto asiente y no
// corte con carga (perilla de calibracion). Excepcion documentada a "delay() solo en Maneuvers".
constexpr unsigned long RELAY_SWITCH_MS = 15;

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
// Distancia que se reporta sin eco (no hay objeto cerca) o fuera de rango: se lee como "despejado"
constexpr long MAX_DISTANCE_CM      = 400;
// Pausa entre disparos de ultrasonicos para que un sensor no lea el eco del anterior.
// Excepcion documentada a "delay() solo en Maneuvers".
constexpr unsigned long ULTRASONIC_GAP_MS = 5;

// Tope de cada ciclo de alineacion con ultrasonicos en Maneuvers (perilla de calibracion).
// Durante una maniobra el watchdog no se revisa (el switch y la senal del control si): sin este
// tope, un sensor fallando haria girar al robot hasta que el operador intervenga.
constexpr unsigned long ALIGN_TIMEOUT_MS = 5000;

// Watchdog del modo autonomo (solo protocolo JSON): sin comando nuevo en este tiempo se detiene
constexpr unsigned long CMD_TIMEOUT_MS = 500;

// Compatibilidad con el script legacy de texto plano (F/L/R/U/S, "T", "LOG: ", "MODE: ").
// Con true no se mandan lecturas de sensores y el watchdog queda desactivado; se conserva
// solo para probar el robot sin el puente ROS2.
// Con false: protocolo JSON de docs/protocols/motors-protocol.md, usado por serial_bridge de
// robot_ws. Las lecturas ultrasonicas se envian a SENSOR_TELEMETRY_MS para no saturar 9600 baud
// (una linea bloquea el loop ~140 ms), y el watchdog de CMD_TIMEOUT_MS queda activo.
constexpr bool LEGACY_PROTOCOL      = false;

// Potencias para motores 
constexpr int SPEED_LEFT_FRONT      = 150;
constexpr int SPEED_LEFT_TURN       = 200;
constexpr int SPEED_RIGHT_TURN      = SPEED_LEFT_TURN;
constexpr int SPEED_RIGHT_FRONT     = SPEED_LEFT_FRONT;