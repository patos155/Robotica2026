#include "communication.h"

void Communication::begin(long baudRate) {
    Serial.begin(baudRate);
    while (!Serial);
}

String Communication::receiveCommand() {
    if (Serial.avaliable()) {
        String rawJson = Serial.readStringUntil('\n');
        rawJson.trim();

        if (rawJson.indexOf("\"value\": \"F\"") != -1) return "F";
        if (rawJson.indexOf("\"value\": \"L\"") != -1) return "L";
        if (rawJson.indexOf("\"value\": \"R\"") != -1) return "R";
        if (rawJson.indexOf("\"value\": \"U\"") != -1) return "U";
        if (rawJson.indexOf("\"value\": \"S\"") != -1) return "S";
    }
    return "";
}

void Communication::sendSensorData(UltrasonicArray * sensors) {
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
    Serail.println("{\"type\": \"status\", \"value\": \"DONE\"}");
}

void Communication::sendMode(String mode) {
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