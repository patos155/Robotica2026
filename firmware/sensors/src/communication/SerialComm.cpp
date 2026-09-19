#include "SerialComm.h"
#include <ArduinoJson.h>
#include "../config.h"
#include "../protocol.h"

void SerialComm::begin() {
    Serial.begin(SERIAL_BAUD_RATE);
}

void SerialComm::update() {
    while (Serial.available() > 0) {
        handleIncomingByte(static_cast<char>(Serial.read()));
    }
}

void SerialComm::handleIncomingByte(char incoming) {
    if (incoming == '\n') {
        _lineBuffer[_lineLength] = '\0';
        handleLine(_lineBuffer);
        _lineLength = 0;
        return;
    }

    // descarta '\r' y bytes que no quepan, en vez de colgarse
    if (incoming != '\r' && _lineLength < LINE_BUFFER_SIZE - 1) {
        _lineBuffer[_lineLength] = incoming;
        _lineLength++;
    }
}

void SerialComm::handleLine(const char* line) {
    StaticJsonDocument<Protocol::JSON_DOC_CAPACITY> doc;
    if (deserializeJson(doc, line)) {
        logWarning(F("JSON invalido recibido por Serial"));
        return;
    }

    const char* type = doc[Protocol::Key::TYPE];
    if (type == nullptr || strcmp(type, Protocol::Type::CMD) != 0) {
        return; // por ahora solo procesamos comandos
    }

    const char* action = doc[Protocol::Key::ACTION];
    if (action == nullptr) {
        return;
    }

    if (strcmp(action, Protocol::Action::READ_SENSORS) == 0) {
        _magnetReadRequested = true;
        return;
    }

    logWarning(F("Accion no soportada todavia"));
}

bool SerialComm::consumeMagnetReadRequest() {
    bool wasRequested = _magnetReadRequested;
    _magnetReadRequested = false;
    return wasRequested;
}

void SerialComm::sendMagnetReading(const MagnetSensor::Reading& reading) {
    if (reading.analog < 0) {
        logWarning(F("MagnetSensor no disponible al pedir lectura"));
        return;
    }

    StaticJsonDocument<Protocol::JSON_DOC_CAPACITY> doc;
    doc[Protocol::Key::TYPE] = Protocol::Type::SENSOR;
    doc[Protocol::Key::SENSOR] = Protocol::Sensor::MAGNET;

    JsonObject data = doc.createNestedObject(Protocol::Key::DATA);
    data[Protocol::Key::DETECTED] = reading.detected;

    switch (reading.pole) {
        case MagnetSensor::Pole::NORTH:
            data[Protocol::Key::POLE] = Protocol::Pole::NORTH;
            break;
        case MagnetSensor::Pole::SOUTH:
            data[Protocol::Key::POLE] = Protocol::Pole::SOUTH;
            break;
        case MagnetSensor::Pole::NONE:
        default:
            data[Protocol::Key::POLE] = Protocol::Pole::NONE;
            break;
    }

    serializeJson(doc, Serial);
    Serial.println();
}

// --- Logging: mismo tipo "log" del protocolo (ver CONVENTIONS.md) ---

void SerialComm::writeLogOpen() {
    Serial.print(F("{\""));
    Serial.print(Protocol::Key::TYPE);
    Serial.print(F("\":\""));
    Serial.print(Protocol::Type::LOG);
    Serial.print(F("\",\""));
    Serial.print(Protocol::Key::VALUE);
    Serial.print(F("\":\""));
}

void SerialComm::writeLogClose() {
    Serial.println(F("\"}"));
}

void SerialComm::logInfo(const __FlashStringHelper* msg) {
#ifdef DEBUG_MODE
    writeLogOpen();
    Serial.print(msg);
    writeLogClose();
#else
    (void)msg;
#endif
}

void SerialComm::logWarning(const __FlashStringHelper* msg) {
    writeLogOpen();
    Serial.print(msg);
    writeLogClose();
}

void SerialComm::logError(const __FlashStringHelper* msg) {
    writeLogOpen();
    Serial.print(msg);
    writeLogClose();
}

void SerialComm::logDebug(const __FlashStringHelper* msg) {
#ifdef DEBUG_MODE
    writeLogOpen();
    Serial.print(msg);
    writeLogClose();
#else
    (void)msg;
#endif
}

void SerialComm::logDebug(const __FlashStringHelper* label, int16_t value) {
#ifdef DEBUG_MODE
    writeLogOpen();
    Serial.print(label);
    Serial.print(value);
    writeLogClose();
#else
    (void)label;
    (void)value;
#endif
}

void SerialComm::logDebug(const __FlashStringHelper* label, bool value) {
#ifdef DEBUG_MODE
    writeLogOpen();
    Serial.print(label);
    Serial.print(value ? F("true") : F("false"));
    writeLogClose();
#else
    (void)label;
    (void)value;
#endif
}
