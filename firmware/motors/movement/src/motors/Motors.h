// =============================================================
// Nombre de la clase:          Motors
// Descripcion:                 Control de motores
// Restricciones importantes:   modulo que interactua con el puente H del robot. Los reles nunca se
//                              conmutan con PWM aplicado (ver move())
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

        // Si cambia el signo de la velocidad de un lado, corta su PWM y conmuta sus reles; devuelve si conmuto
        bool _switchRelays(int speed, int currentSpeed, int pinPWM, int pinRelay1, int pinRelay2);
};
