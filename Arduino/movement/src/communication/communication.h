// =============================================================
// Nombre de la clase:          Communication
// Descripcion:                 Gestion de la comunicacion Serial con Python
// Restricciones importantes:   Maneja envio de JSON y lectura de comandos
// =============================================================

#pragma once
#include <Arduino.h>

class Communication {
    public:
        void begin(long baudRate=9600);
        String receiveCommand();
        void sendSensorData(UltrasonicArray* sensors);
        void sendSecuenceDone();
        void sendMode(String mode);
        void sendLog(String message);
}