// =============================================================
// Nombre de la clase:          Maneuvers
// Descripcion:                 Movimiento en modo autonomo
// Restricciones importantes:   Debe recibir referencias de Motors y UltrasonicArray
// =============================================================

#pragma once
#include <Arduino.h>
#include "motors.h"
#include "UltrasonicArray.h"

class Maneuvers() {
    public:
        void begin(Motors * motorsInstance, UltrasonicArray * sensorsInstance);
        void turnLeft();
        void turnRight();
        void uTurn();
        constexpr int delayStop = 1000;
        constexpr int delayTurn = 1500;
        constexpr int delayForward = 1500;
        constexpr sequenceComplete = "T";

    private:
        Motors* _motors;
        UltrasonicArray* _sensors;
}