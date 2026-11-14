// =============================================================
// Nombre de la clase:          Maneuvers
// Descripcion:                 Movimiento en modo autonomo
// Restricciones importantes:   Debe recibir referencias de Motors y UltrasonicArray
// =============================================================

#pragma once
#include <Arduino.h>
#include "../motors/Motors.h"
#include "../sensors/Ultrasonic/UltrasonicArray.h"
#include "../communication/SerialComm.h"

class Maneuvers {
    public:
        void begin(Motors * motorsInstance, UltrasonicArray * sensorsInstance, Communication * comm);
        void turnLeft();
        void turnRight();
        void uTurn();
        
        // Tiempos de las secuencias en ms, iguales a legacy.ino
        static constexpr int DELAY_STOP             = 1000;  // pausas de paro, iguales en las tres maniobras
        static constexpr int RIGHT_TURN_MS          = 1500;  // giro inicial
        static constexpr int RIGHT_FINAL_PAUSE_MS   = 1500;
        static constexpr int LEFT_TURN_MS           = 1000;
        static constexpr int LEFT_FINAL_PAUSE_MS    = 1000;
        static constexpr int U_TURN_MS              = 4000;
        static constexpr int U_FINAL_PAUSE_MS       = 1000;

    private:
        // Crea punteros para las clases
        Motors* _motors;
        UltrasonicArray* _sensors;
        Communication* _comm;

        void _align(bool useRightSide, int leftSpeed, int rightSpeed);
        void _finish(int finalPauseMs);
};