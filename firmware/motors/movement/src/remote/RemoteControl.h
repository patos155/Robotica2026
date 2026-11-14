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
        // Solo el canal del switch (ch4): ~20 ms contra >120 ms de los 6. Para consultar durante una maniobra
        void updateModeChannel();
        bool isSignalLost() const;
        bool isAutonomousMode() const;
        float getChannelValue(int channel) const;

    private:
        int _rcPins[6];
        float _chValue[6];
        uint8_t _lostReads = 0;
        void _readChannel(int channel);
        float _fmap(float x, float in_min, float in_max, float out_min, float out_max);
};