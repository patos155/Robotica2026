// =============================================================
// Nombre de la clase:          Maneuvers
// Descripcion:                 Movimiento en modo autonomo
// Restricciones importantes:   Debe recibir referencias de Motors y UltrasonicArray
// =============================================================

#pragma once
#include <Arduino.h>
#include "../motors/motors.h"
#include "../sensors/Ultrasonic/UltrasonicArray.h"
#include "../communication/SerialComm.h"

class Maneuvers {
    public:
        void begin(Motors * motorsInstance, UltrasonicArray * sensorsInstance, Communication * comm);
        void turnLeft();
        void turnRight();
        void uTurn();
        
        // tiempo de los delays para las secuencias
        static constexpr int DELAY_STOP = 1000;
        static constexpr int DELAY_TURN = 1500;
        static constexpr int DELAY_FORWARD = 1500;
        static constexpr int DELAY_U_TURN = 4000;

    private:
        // Crea punteros para las clases
        Motors* _motors;
        UltrasonicArray* _sensors;
        Communication* _comm;
};