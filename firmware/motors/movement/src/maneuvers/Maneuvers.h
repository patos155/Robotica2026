// =============================================================
// Nombre de la clase:          Maneuvers
// Descripcion:                 Movimiento en modo autonomo
// Restricciones importantes:   Debe recibir punteros de Motors, UltrasonicArray y Communication, y la
//                              funcion de aborto (definida en movement.ino: Maneuvers no depende de Remote)
// =============================================================

#pragma once
#include <Arduino.h>
#include "../motors/Motors.h"
#include "../sensors/Ultrasonic/UltrasonicArray.h"
#include "../communication/SerialComm.h"

class Maneuvers {
    public:
        // shouldAbort se consulta en cada espera y en cada vuelta de la alineacion; si devuelve true
        // (switch a manual o control perdido) la maniobra se detiene en ese momento
        void begin(Motors * motorsInstance, UltrasonicArray * sensorsInstance, Communication * comm, bool (*shouldAbort)());
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
        bool (*_shouldAbort)();

        void _turn(int leftSpeed, int rightSpeed, unsigned long turnMs, bool useRightSide, unsigned long finalPauseMs);
        bool _wait(unsigned long ms);
        bool _align(bool useRightSide, int leftSpeed, int rightSpeed);
};
