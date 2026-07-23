// =============================================================
// Nombre de la clase:          Logger
// Descripcion:                 Depuracion general del sistema, incluyendo sensores y control remoto
// Restricciones importantes:   Unico modulo que puede depurar el sistema
// =============================================================

#pragma once
#include <Arduino.h>
#include "./communication/SerialComm.h"
#include "./remote/RemoteControl.h"
#include "./sensors/Ultrasonic/UltrasonicArray.h"

class Logger {
private:
    Communication* _comm;
    bool _enabled;

public:
    Logger();
    void begin(Communication* comm, bool enable = false);

    void info(const __FlashStringHelper* msg);
    void warning(const __FlashStringHelper* msg);
    void error(const __FlashStringHelper* msg);
    
    void debug(const __FlashStringHelper* msg);
    void debug(const __FlashStringHelper* msg, int value);
    void debug(const __FlashStringHelper* msg, float value);

    // Funciones específicas para lectura masiva
    void printChannels(Remote& rc);
    void printSensors(UltrasonicArray& sensors);
};