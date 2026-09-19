#include "MagnetSensor.h"
#include "config.h"

void MagnetSensor::begin() {
    _isReady = true; // pines analógicos no necesitan pinMode
}

MagnetSensor::Reading MagnetSensor::read() const {
    if (!_isReady) {
        return Reading{-1, false, Pole::NONE};
    }

    int16_t analog = analogRead(PIN_MAGNET_ANALOG);
    bool detected = abs(analog - MAGNET_QUIESCENT_ADC) > MAGNET_DEADBAND_ADC;

    Pole pole = Pole::NONE;
    if (detected) {
        bool isAbove = analog > MAGNET_QUIESCENT_ADC;
        pole = (isAbove == MAGNET_ABOVE_QUIESCENT_IS_NORTH) ? Pole::NORTH : Pole::SOUTH;
    }

    return Reading{analog, detected, pole};
}
