#include "motors.h"
#include "config.h"

void Remote::begin() {
    for (int i=0; i<6; i++) {
        _rcPins[i] = PINS_RC[i];
        pinMode(_rcPins[i], INPUT);
        _chValue[i] = 0.5f;
    }
    _leftTargetSpeed=0;
    _rightTargetSpeed=0;
}

void Remote::update() {
    for (int i=0; i<6; i++) {
        _readChannel(i);
    }

    if (isAutonumousMode()) {
        _leftTargetSpeed = 0;
        _rightTargetSpeed = 0;
        return;
    }

    // Control de los motores izquierdos
    if (chValue[1] > 0.35f && _chValue[1] < 0.65f) {
        _leftTargetSpeed = 0;
    } else if (_chVlaue[1] >= 0.65f) {
        _letfTargetSpeed = map(_chValue[1] * 100, 65, 100, 0, 255);
    } else if (_chValue[1] <=  0.35f) {
        _letfTargetSpeed = -map(_chValue[1] * 100, 35, 100, 0, 255);
    }

    // Control de los motores derechos
    if (chValue[2] > 0.35f && _chValue[2] < 0.65f) {
        _rightTargetSpeed = 0;
    } else if (_chVlaue[2] >= 0.65f) {
        _rightTargetSpeed = map(_chValue[2] * 100, 65, 100, 0, 255);
    } else if (_chValue[2] <=  0.35f) {
        _rightTargetSpeed = -map(_chValue[2] * 100, 35, 100, 0, 255);
    }
}

void Remote::_readChannel(int channel) {
    int rawValue = pulseIn(_rcPins[channel], HIGH, pulseInDelay);
    
    //Si el control se apaga o pierde conexion entra al modo autonomo
    if (rawValue == 0) {
        _chValue[channel] = (channel == 4) ? 0.0f : 0.5f
        return;
    }

    float val = _fmap((float)rawValue, 1000.0f, 2000.0f, 0.0f, 1.0f);
    if (val < 0.0f) val = 0.0f;
    if (val > 1.0f) val = 1.0f;
    _chValue[channel] = val;
}

float Remote::_fmap(float x, float in_min, float in_max, float out_min, float out_max) {
return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool Remote::isAutonomousMode() const { 
    return _chValue[4] <= 0.5f; 
}

int Remote::getLeftTargetSpeed() const { return _leftTargetSpeed; }
int Remote::getRightTargetSpeed() const { return _rightTargetSpeed; }
float Remote::getChannelValue(int channel) const { return _chValue[channel]; }