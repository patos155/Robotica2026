// =============================================================
// Nombre de la clase:          Maneuvers
// Descripcion:                 Movimiento en modo autonomo
// Restricciones importantes:   Debe recibir referencias de Motors y UltrasonicArray
// =============================================================

#pragma once
#include <Arduino.h>
#include "motors.h"
#include "UltrasonicArray.h"

class Maneuvers {
    public:
        void begin(Motors * motorsInstance, UltrasonicArray * sensorsInstance);
        void update();
        void turnLeft();
        void turnRight();
        void uTurn();
        
        static constexpr int delayStop = 1000;
        static constexpr int delayTurn = 1500;
        static constexpr int delayForward = 1500;
        static constexpr int delayUTurn = 4000;

    private:
        Motors* _motors;
        UltrasonicArray* _sensors;
};