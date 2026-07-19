#include "./remote/RemoteControl.h"
#include "config.h"

void Remote::begin() {
    for (int i=0; i<6; i++) {
        _rcPins[i] = PINS_RC[i];
        pinMode(_rcPins[i], INPUT);
        _chValue[i] = 0.5f;
    }
}

void Remote::update() {
    // Lee constantemente los canales del control remoto
    for (int i=0; i<6; i++) {
        _readChannel(i);
    }
}

void Remote::_readChannel(int channel) {
    // Lee la duración del pulso (en microsegundos) en el pin especificado para el canal dado
    // 'rcPins' es un array que contiene los pines de entrada para cada canal
    // 'HIGH' especifica que estamos midiendo la duración del pulso alto
    // 'pulseInDelay' es el tiempo máximo que esperamos a que se complete el pulso
    int rawValue = pulseIn(_rcPins[channel], HIGH, pulseInDelay);
    
    //Si el control se apaga o pierde conexion entra al modo autonomo
    if (rawValue == 0) {
        _chValue[channel] = (channel == 4) ? 0.0f : 0.5f;
        return;
    }

    // Convierte el valor leído (rawValue) de un rango de 1000 a 2000 microsegundos a un rango de 0.0 a 1.0
    float val = _fmap((float)rawValue, 1000.0f, 2000.0f, 0.0f, 1.0f);
    if (val < 0.0f) val = 0.0f;
    if (val > 1.0f) val = 1.0f;
    _chValue[channel] = val;
}

// Función para mapear un valor de un rango a otro rango
float Remote::_fmap(float x, float in_min, float in_max, float out_min, float out_max) {
return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool Remote::isAutonomousMode() const { 
    return _chValue[4] <= 0.5f; 
}

float Remote::getChannelValue(int channel) const { 
    return _chValue[channel]; 
}