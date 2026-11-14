#include "SerialComm.h"
#include "../../protocol.h"
#include "../../config.h"

void Communication::begin(long baudRate) {
    Serial.begin(baudRate);
    while (!Serial);
    // readStringUntil espera hasta este tiempo si la linea llega sin '\n'; el default (1 s) congela el loop
    Serial.setTimeout(50);
}

String Communication::receiveCommand() {
    // Recibe del python los comandos F = Adelante, L = Izquierda, R = Derecha, U = Vuelta en U, S = alto
    if (!Serial.available()) return "";
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();     // tolera "\r\n"
    if (cmd.length() == 0) return "";

    if (!LEGACY_PROTOCOL) {
        // {"type": "cmd", "value": "F"} -> F
        int key = cmd.indexOf("\"value\"");
        int colon = (key < 0) ? -1 : cmd.indexOf(':', key);
        int start = (colon < 0) ? -1 : cmd.indexOf('"', colon);
        int end = (start < 0) ? -1 : cmd.indexOf('"', start + 1);
        cmd = (end < 0) ? "" : cmd.substring(start + 1, end);
    }

    if (cmd.length() == 1) {
        char c = cmd[0];
        if (c == Cmd::cmdForward || c == Cmd::cmdLeft || c == Cmd::cmdRight ||
            c == Cmd::cmdUturn || c == Cmd::cmdStop) return cmd;
    }
    // Comando desconocido: se detiene, como legacy
    sendLog(F("Comando desconocido, motores detenidos"));
    return String(Cmd::cmdStop);
}

void Communication::flushInput() {
    // Descarta lo que haya llegado mientras el loop estaba bloqueado (p. ej. durante una maniobra)
    while (Serial.available()) Serial.read();
}

void Communication::sendSensorData(UltrasonicArray * sensors) {
    // Legacy no manda sensores: a 9600 baud esta linea bloquea el loop 90-150 ms
    if (LEGACY_PROTOCOL) return;

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
    if (LEGACY_PROTOCOL) {
        Serial.println(Status::DoneLegacy);
        return;
    }
    Serial.print("{\"type\": \"status\", \"value\": \"");
    Serial.print(Status::Done);
    Serial.println("\"}");
}

void Communication::sendMode(String mode) {
    // Notifica a Python cuando cambian con el switch del control
    Serial.print("{\"type\": \"mode\", \"value\": \"");
    Serial.print(mode);
    Serial.println("\"}");
}

void Communication::sendLog(String message) {
    // Para mandar textos de debug
    if (LEGACY_PROTOCOL) {
        // El prefijo garantiza que Python no la tome como sensores ("{...}") ni como fin de maniobra ("T")
        Serial.print(F("LOG: "));
        Serial.println(message);
        return;
    }
    Serial.print("{\"type\": \"log\", \"value\": \"");
    Serial.print(message);
    Serial.println("\"}");
}
