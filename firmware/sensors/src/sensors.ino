// =============================================================
// robot_rescate_sensors.ino
// setup()/loop() — solo orquestación (ver CONVENTIONS.md). No
// contiene lógica de negocio: solo crea objetos, llama begin() y
// update() de cada módulo.
// =============================================================

#include "communication/SerialComm.h"
SerialComm comm;
void setup() {
    comm.begin();
    comm.logInfo(F("Sistema iniciado"));
}

void loop() {
    comm.update();
}
