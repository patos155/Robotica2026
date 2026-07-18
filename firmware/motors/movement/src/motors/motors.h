// =============================================================
// Nombre de la clase:          Motors
// Descripcion:                 Control de motores
// Restricciones importantes:   modulo que interactua con el puente H del robot
// =============================================================

#pragma once
#include <Arduino.h>

class Motors {
    public:
        void begin();
        void move(int leftSpeed, int rightSpeed);
        void stop();

    private:
        int _currentLeftSpeed;
        int _currentRightSpeed;

        // Metodo privado para interactuar con el puente H
        void _setMotorsPins(int speed, int pinPWM, int pinRelay1, int pintRelay2);
};