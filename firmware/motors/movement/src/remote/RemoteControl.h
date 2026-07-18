// =============================================================
// Nombre de la clase:          Remote
// Descripcion:                 Lectura de señales del control remoto
// Restricciones importantes:   modulo que interactua con el receptor del control remoto
// =============================================================

#pragma once
#include <Arduino.h>

class Remote {
    public:
        void begin();
        void update();
        int getLeftTargetSpeed() const;
        int getRightTargetSpeed() const;
        bool isAutonomousMode() const;
        float getChannelValue(int channel) const;

    private:
        int _leftTargetSpeed;
        int _rightTargetSpeed;
        int _rcPins[6];
        float _chValue[6];
        void _readChannel(int channel);
        float _fmap(float x, float in_min, float in_max, float out_min, float out_max);
};