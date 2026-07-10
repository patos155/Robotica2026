#pragma once

#include <Arduino.h>

// Variables de distancia minima a paredes 
constexpr int lateralDistance = 25;                        // Distancia lateral
constexpr int frontDistance = 25;                         // Distancia delantera

// Sensores ultrasonicos
// conexiones de triger y echo Izquierdo trasero 
constexpr int leftRearTrigger = 41;           
constexpr int leftRearEcho = 42;
// conexiones de triger y echo Izquierdo delantero
constexpr int leftFrontTrigger = 5;
constexpr int leftFrontEcho = 4;
// conexiones de triger y echo delantero izquierdo
constexpr int frontLeftTrigger = 19;
constexpr int frontLeftEcho = 20;
// conexiones de triger y echo delantero derecho
constexpr int frontRightTrigger = 8;
constexpr int frontRightEcho = 40;
// conexiones de triger y echo derecho delantero
constexpr int rightFrontTrigger = 21;
constexpr int rightFrontEcho = 22;
// conexiones de triger y echo derecho trasero
constexpr int rightRearTrigger = 23;
constexpr int rightRearEcho = 24;

// variables para sensores ultrasonicos 
constexpr long LRM,LRM,FLM,FRM,RFM,RRM; // Mediciones de sensores ultrasonicos en centimetros 
constexpr int LRL,LFL,FLL,FRL,RFL,RRL; // Valores logicos de lectura de sensores ultrasonicos

constexpr int pulseInDelay = 30000;