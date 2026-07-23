#include "System.h"
#include "config.h"

Logger::Logger() {}

void Logger::begin(Communication* comm, bool enabled) {
    _comm = comm;
    _enabled = enabled;
}

void Logger::info(const __FlashStringHelper* msg) {
#ifdef DEBUG_MODE
    if (!_enabled) return;
    _comm->sendLog(String("[INFO]: ") + String(msg));
#endif
}

void Logger::warning(const __FlashStringHelper* msg) {
    _comm->sendLog(String("[ADVERTENCIA] ! : ") + String(msg));
}

void Logger::error(const __FlashStringHelper* msg) {
    _comm->sendLog(String("[ERROR] X : ") + String(msg));
}

void Logger::debug(const __FlashStringHelper* msg) {
#ifdef DEBUG_MODE
    if (!_enabled) return;
    _comm->sendLog(String("[DEBUG]: ") + String(msg));
#endif
}

void Logger::debug(const __FlashStringHelper* msg, int value) {
#ifdef DEBUG_MODE
    if (!_enabled) return;
    _comm->sendLog(String("[DEBUG]: ") + String(msg) + String(value));
#endif
}

void Logger::debug(const __FlashStringHelper* msg, float value) {
#ifdef DEBUG_MODE
    if (!_enabled) return;
    _comm->sendLog(String("[DEBUG]: ") + String(msg) + String(value));
#endif
}

void Logger::printChannels(Remote& rc) {
#ifdef DEBUG_MODE
    if (!_enabled) return;
    _comm->sendLog(F("--- Canales CR ---"));
    for (int iChannel = 1; iChannel <= 6; iChannel++) {
        float value = rc.getChannelValue(iChannel);
        String message = "Ch #" + String(iChannel) + ": " + String(value);
        _comm->sendLog(message);
    }
    _comm->sendLog(F("------------------"));
#endif
}
void Logger::printSensors(UltrasonicArray& sensors) {
#ifdef DEBUG_MODE
    if (!_enabled) return;
    String data = "Ultrasonic -> fr-Izq: " + String(sensors.getFLM()) + " | fr-Der: " + String(sensors.getFRM()) + " | izq-Fr: " + String(sensors.getLFM()) + "| izq-Tra" + String(sensors.getLRM()) + " | der-Fr: " + String(sensors.getRFM()) + "| der-Tra" + String(sensors.getRRM());
    _comm->sendLog(data);
#endif
}