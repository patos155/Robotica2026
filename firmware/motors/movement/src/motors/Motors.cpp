#include "Motors.h"
#include "../../config.h"

void  Motors::begin(){
    pinMode(PIN_PWM_LEFT, OUTPUT);
    pinMode(PIN_RELAY_LEFT_1, OUTPUT);
    pinMode(PIN_RELAY_LEFT_2, OUTPUT);

    pinMode(PIN_PWM_RIGHT, OUTPUT);
    pinMode(PIN_RELAY_RIGHT_1, OUTPUT);
    pinMode(PIN_RELAY_RIGHT_2, OUTPUT);

    // Distinto de 0 para que stop() vea un cambio de signo y escriba los reles al arrancar
    _currentLeftSpeed = _currentRightSpeed = 1;
    stop();
}

// Conmutar un rele con PWM aplicado lo hace cortar con carga (arco en los contactos). Si cambia el
// signo de un lado (adelante/atras/paro): PWM a 0, conmutar reles, esperar RELAY_SWITCH_MS a que
// asienten y despues aplicar el PWM. Una sola pausa aunque cambien los dos lados, para que arranquen
// juntos. Si el signo no cambia solo se actualiza el PWM, sin pausa.
void Motors::move(int leftSpeed, int rightSpeed) {
    bool leftSwitched  = _switchRelays(leftSpeed, _currentLeftSpeed, PIN_PWM_LEFT, PIN_RELAY_LEFT_1, PIN_RELAY_LEFT_2);
    bool rightSwitched = _switchRelays(rightSpeed, _currentRightSpeed, PIN_PWM_RIGHT, PIN_RELAY_RIGHT_1, PIN_RELAY_RIGHT_2);
    if (leftSwitched || rightSwitched) delay(RELAY_SWITCH_MS);

    // Limitar que la velocidad nunca supere el rango de 0-255 de los timers de Arduino
    analogWrite(PIN_PWM_LEFT, min(abs(leftSpeed), 255));
    analogWrite(PIN_PWM_RIGHT, min(abs(rightSpeed), 255));

    _currentLeftSpeed = leftSpeed;
    _currentRightSpeed = rightSpeed;
}

void Motors::stop() {
    move(0, 0);
}

bool Motors::_switchRelays(int speed, int currentSpeed, int pinPWM, int pinRelay1, int pinRelay2) {
    if ((speed > 0) == (currentSpeed > 0) && (speed < 0) == (currentSpeed < 0)) return false;
    analogWrite(pinPWM, 0);
    digitalWrite(pinRelay1, speed > 0 ? RELAY_ON : RELAY_OFF);     // adelante
    digitalWrite(pinRelay2, speed < 0 ? RELAY_ON : RELAY_OFF);     // atras
    return true;
}
