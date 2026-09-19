#pragma once

#include <Arduino.h>

// =============================================================
// MagnetSensor
// Lee el Hall lineal SS49E: presencia y polo de un imán. Sin
// dependencias del proyecto (solo config.h). No decide qué hacer
// con la lectura — eso vive en robot_rescate_sensors.ino.
// =============================================================

class MagnetSensor {
public:
    enum class Pole : uint8_t { NONE, NORTH, SOUTH };

    struct Reading {
        int16_t analog; // ADC crudo, -1 si !isReady()
        bool detected;
        Pole pole; // NONE si !detected
    };

    void begin();
    bool isReady() const { return _isReady; }

    // Una sola lectura de ADC → detected/pole consistentes entre sí.
    Reading read() const;

private:
    bool _isReady = false;
};
