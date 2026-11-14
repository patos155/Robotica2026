#include "SerialComm.h"
#include "../../protocol.h"
#include "../../config.h"

void Communication::begin(long baudRate) {
    Serial.begin(baudRate);
    while (!Serial);
    // readStringUntil espera hasta este tiempo si la linea llega sin '\n'; el default (1 s) congela el loop
    Serial.setTimeout(50);
}

char Communication::receiveCommand() {
    // Recibe del python los comandos F = Adelante, L = Izquierda, R = Derecha, U = Vuelta en U, S = alto
    if (!Serial.available()) return '\0';

    // Cabe el JSON del protocolo (~30 bytes).
    // ponytail: una linea mas larga se parte y cada parte cuenta como comando desconocido (paro)
    constexpr size_t CMD_BUFFER_SIZE = 40;
    char line[CMD_BUFFER_SIZE];
    line[Serial.readBytesUntil('\n', line, CMD_BUFFER_SIZE - 1)] = '\0';

    // trim: tolera "\r\n" y espacios
    char* cmd = line;
    while (isspace(*cmd)) cmd++;
    char* end = cmd + strlen(cmd);
    while (end > cmd && isspace(end[-1])) *--end = '\0';
    if (*cmd == '\0') return '\0';

    if (!LEGACY_PROTOCOL) {
        // {"type": "cmd", "value": "F"} -> F
        char* key = strstr(cmd, "\"value\"");
        char* colon = key ? strchr(key, ':') : nullptr;
        char* start = colon ? strchr(colon, '"') : nullptr;
        char* close = start ? strchr(start + 1, '"') : nullptr;
        if (close) {
            *close = '\0';
            cmd = start + 1;
        } else {
            *cmd = '\0';
        }
    }

    if (cmd[0] != '\0' && cmd[1] == '\0') {
        switch (cmd[0]) {
            case Cmd::cmdForward:
            case Cmd::cmdLeft:
            case Cmd::cmdRight:
            case Cmd::cmdUturn:
            case Cmd::cmdStop:
                return cmd[0];
        }
    }
    // Comando desconocido: se detiene, como legacy
    sendLog(F("Comando desconocido, motores detenidos"));
    return Cmd::cmdStop;
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
