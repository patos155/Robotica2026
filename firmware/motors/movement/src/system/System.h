// =============================================================
// Nombre de la clase:          System
// Descripcion:                 Depuracion general del sistema, incluyendo sensores y control remoto
// Restricciones importantes:   Unico modulo que puede depurar el sistema
// =============================================================

#pragma once
#include <Arduino.h>
#include "./communication/SerialComm.h"
#include "./remote/RemoteControl.h"
#include "./sensors/Ultrasonic/UltrasonicArray.h"

class System {
private:
    Communication* _comm;
    bool _enabled;

public:
    System();
    void begin(Communication* comm, bool enabled = false); 
    void setEnabled(bool enabled);
    bool isEnabled();
    void info(String message);
    void warning(String message);
    void error(String message);
    void printChannels(Remote& rc);
    void printSensors(UltrasonicArray& sensors);
};