// =============================================================
// robot_rescate_sensors.ino
// setup()/loop() — solo orquestación (ver CONVENTIONS.md). No
// contiene lógica de negocio: solo crea objetos, llama begin() y
// update() de cada módulo.
// =============================================================

#include "config.h"
#include "sensors/magnet/MagnetSensor.h"
#include "communication/SerialComm.h"

SerialComm comm;
MagnetSensor magnetSensor;

void setup() {
    comm.begin();
    magnetSensor.begin();
    comm.logInfo(F("Sistema iniciado"));
}

void loop() {
    comm.update();

    if (comm.consumeMagnetReadRequest()) {
        MagnetSensor::Reading reading = magnetSensor.read();
        comm.sendMagnetReading(reading);
    }
}
