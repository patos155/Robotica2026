#include "SerialComm.h"
#include "protocol.h"

void Communication::begin(long baudRate) {
    Serial.begin(baudRate);
    while (!Serial);
}

String Communication::receiveCommand() {
    // Recibe del python los comandos F = Adelante, L = Izquierda, R = Derecha, U = Vuelta en U, S = alto
    if (Serial.avaliable()) {
        String rawJson = Serial.readStringUntil('\n');

        if (rawJson.indexOf(Cmd::cmdForward) != -1) return "F";
        if (rawJson.indexOf(Cmd::cmdLeft) != -1) return "L";
        if (rawJson.indexOf(Cmd::cmdRight) != -1) return "R";
        if (rawJson.indexOf(Cmd::cmdUturn) != -1) return "U";
        if (rawJson.indexOf(Cmd::cmdStop) != -1) return "S";
    }
    return "";
}

void Communication::sendSensorData(UltrasonicArray * sensors) {
    // Le envia a Python las mediciones de los sensores ultrasonicos
    Serial.print("{\"type\": \"sensor\", \"sensor\": \"ultrasonic\", \"data\": {");
    
    Serial.print("\"distFL\": "); Serial.print(sensors->getFLM()); Serial.print(", ");
    Serial.print("\"distFR\": "); Serial.print(sensors->getFRM()); Serial.print(", ");
    Serial.print("\"distLF\": "); Serial.print(sensors->getLFM()); Serial.print(", ");
    Serial.print("\"distLR\": "); Serial.print(sensors->getLRM()); Serial.print(", ");
    Serial.print("\"distRF\": "); Serial.print(sensors->getRFM()); Serial.print(", ");
    Serial.print("\"distRR\": "); Serial.print(sensors->getRRM());
    
    Serial.println("}}");
}

void Communication::sendStatusDone() {
    // Notifica a Python cuando termina una secuencia de giro
    Serial.print("{\"type\": \"status\", \"value\": \"");
    Serial.print(Status::Done);
    Serial.println("\"}");
}

void Communication::sendMode(constexpr char* mode) {
    // Notifica a Python cuando cambian con el switch del control
    Serial.print("{\"type\": \"mode\", \"value\": \"");
    Serial.print(mode);
    Serial.println("\"}");
}

void Communication::sendLog(String message) {
    // Para mandar textos de debug
    Serial.print("{\"type\": \"log\", \"value\": \"");
    Serial.print(message);
    Serial.println("\"}");
}