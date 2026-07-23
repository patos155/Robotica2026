#include "System.h"

System::System() {
    _enabled = false;
}

void System::begin(Communication* comm, bool enabled) {
    _comm = comm;
    _enabled = enabled;
}

void System::setEnabled(bool enabled) {
    _enabled = enabled;
}

bool System::isEnabled() {
    return _enabled;
}

void System::info(String message) {
    if (!_enabled) return;
    _comm->sendLog("[INFO]: " + message);
}

void System::warning(String message) {
    if (!_enabled) return;
    _comm->sendLog("[ADVERTENCIA] ! : " + message);
}

void System::error(String message) {
    if (!_enabled) return;
    _comm->sendLog("[ERROR] X : " + message);
}

void System::printChannels(Remote& rc) {
    if (!_enabled) return;
    
    _comm->sendLog("--- Canales CR ---");
    for (int iChannel = 1; iChannel <= 6; iChannel++) {
        float value = rc.getChannelValue(iChannel);
        String message = "Ch #" + String(iChannel) + ": " + String(value);
        _comm->sendLog(message);
    }
    _comm->sendLog("------------------");
}
void System::printSensors(UltrasonicArray& sensors) {
    if (!_enabled) return;
    String data = "Ultrasonic -> fr-Izq: " + String(sensors.getFLM()) + " | fr-Der: " + String(sensors.getFRM()) + " | izq-Fr: " + String(sensors.getLFM()) + "| izq-Tra" + String(sensors.getLRM()) + " | der-Fr: " + String(sensors.getRFM()) + "| der-Tra" + String(sensors.getRRM());
    _comm->sendLog(data);
}