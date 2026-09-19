#pragma once

#include <Arduino.h>
#include "../sensors/magnet/MagnetSensor.h"

// =============================================================
// SerialComm
// Único módulo que lee/escribe Serial, incluyendo logging (ver
// CONVENTIONS.md — no existe clase Logger separada). No lee
// sensores directamente: recibe la lectura ya hecha desde el .ino.
// =============================================================

class SerialComm {
public:
    void begin();
    void update(); // no bloquea, llamar en cada loop()

    // true si llegó "read_sensors" desde la última llamada (se
    // resetea al leerlo, para no reenviar la misma lectura dos veces).
    bool consumeMagnetReadRequest();
    void sendMagnetReading(const MagnetSensor::Reading& reading);

    // Logging vía protocolo (tipo "log"), no Serial.println() directo.
    // INFO/DEBUG se compilan fuera sin DEBUG_MODE; WARNING/ERROR
    // siempre se envían. msg/label deben ser F() literales fijos —
    // sin comillas ni backslashes, no se escapan caracteres especiales.
    void logInfo(const __FlashStringHelper* msg);
    void logWarning(const __FlashStringHelper* msg);
    void logError(const __FlashStringHelper* msg);
    void logDebug(const __FlashStringHelper* msg);
    void logDebug(const __FlashStringHelper* label, int16_t value);
    void logDebug(const __FlashStringHelper* label, bool value);

private:
    static constexpr uint8_t LINE_BUFFER_SIZE = 64;

    char _lineBuffer[LINE_BUFFER_SIZE];
    uint8_t _lineLength = 0;
    bool _magnetReadRequested = false;

    void handleIncomingByte(char incoming);
    void handleLine(const char* line);
    void writeLogOpen();  // {"type":"log","value":"
    void writeLogClose(); // "}
};
