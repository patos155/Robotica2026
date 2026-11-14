#include "Motors.h"
#include "../../config.h"

void  Motors::begin(){
    pinMode(PIN_PWM_LEFT, OUTPUT);
    pinMode(PIN_RELAY_LEFT_1, OUTPUT);
    pinMode(PIN_RELAY_LEFT_2, OUTPUT);

    pinMode(PIN_PWM_RIGHT, OUTPUT);
    pinMode(PIN_RELAY_RIGHT_1, OUTPUT);
    pinMode(PIN_RELAY_RIGHT_2, OUTPUT);

    stop();
}

void Motors::move(int leftSpeed, int rightSpeed) {
    _currentLeftSpeed = leftSpeed;
    _currentRightSpeed = rightSpeed;

    // Enviar las velocidades reales a los pines fisicos
    _setMotorsPins(_currentLeftSpeed, PIN_PWM_LEFT, PIN_RELAY_LEFT_1,  PIN_RELAY_LEFT_2);
    _setMotorsPins(_currentRightSpeed, PIN_PWM_RIGHT, PIN_RELAY_RIGHT_1, PIN_RELAY_RIGHT_2);
}

void Motors::stop() {
    move(0, 0);
}

void Motors::_setMotorsPins(int speed, int pinPWM, int pinRelay1, int pinRelay2) {
    if (speed > 0) {
        digitalWrite(pinRelay1, RELAY_ON);
        digitalWrite(pinRelay2, RELAY_OFF);
    } else if (speed < 0) {
        digitalWrite(pinRelay1, RELAY_OFF);
        digitalWrite(pinRelay2, RELAY_ON);
        speed = -speed; 
    } else {
        digitalWrite(pinRelay1, RELAY_OFF);
        digitalWrite(pinRelay2, RELAY_OFF);
    }

    // Limitar que la velocidad nunca supere el rango de 0-255 de los timers de Arduino
    if (speed > 255) speed = 255;

    analogWrite(pinPWM, speed);
}