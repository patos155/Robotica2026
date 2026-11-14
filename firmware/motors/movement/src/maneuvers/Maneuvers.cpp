#include "Maneuvers.h"
#include "../../config.h"

void Maneuvers::begin(Motors * motorsInstance, UltrasonicArray * sensorsInstance, Communication* comm, bool (*shouldAbort)()) {
    _motors = motorsInstance;
    _sensors = sensorsInstance;
    _comm = comm;
    _shouldAbort = shouldAbort;
}

// Giro hacia la derecha (legacy: vueltaDerecha). Izquierdas adelante, derechas atras; alinea con while (LRM > LFM)
void Maneuvers::turnRight() {
    _turn(SPEED_LEFT_TURN, -SPEED_RIGHT_TURN, RIGHT_TURN_MS, false, RIGHT_FINAL_PAUSE_MS);
}

// Giro hacia la izquierda (legacy: vueltaIzquierda). Izquierdas atras, derechas adelante; alinea con while (RRM > RFM)
void Maneuvers::turnLeft(){
    _turn(-SPEED_LEFT_TURN, SPEED_RIGHT_TURN, LEFT_TURN_MS, true, LEFT_FINAL_PAUSE_MS);
}

// Vuelta en U girando a la izquierda (legacy: vueltaU); alinea con while (RRM > RFM)
void Maneuvers::uTurn(){
    _turn(-SPEED_LEFT_TURN, SPEED_RIGHT_TURN, U_TURN_MS, true, U_FINAL_PAUSE_MS);
}

// Secuencia comun de las tres maniobras de legacy: alto, giro por tiempo, alto, alineacion con
// ultrasonicos, alto y pausa final. Si _shouldAbort() lo pide, detiene los motores y salta el resto,
// pero igual manda el fin de maniobra: Python queda bloqueado esperando "T" y sin el no volveria a
// mandar comandos.
void Maneuvers::_turn(int leftSpeed, int rightSpeed, unsigned long turnMs, bool useRightSide, unsigned long finalPauseMs) {
    _motors->stop();
    bool aborted = _wait(DELAY_STOP);
    if (!aborted) { _motors->move(leftSpeed, rightSpeed); aborted = _wait(turnMs); }
    if (!aborted) { _motors->stop(); aborted = _wait(DELAY_STOP); }
    if (!aborted) aborted = _align(useRightSide, leftSpeed, rightSpeed);
    if (!aborted) { _motors->stop(); aborted = _wait(DELAY_STOP); }
    // Legacy ponia los 4 reles en HIGH: en un medio puente de reles eso frena el motor, asi que en
    // la practica es una pausa. Despues Python manda "F" por su cuenta. Si en el robot se confirma
    // que legacy avanzaba en este paso, basta agregar antes de esta espera
    // _motors->move(SPEED_LEFT_FRONT, SPEED_RIGHT_FRONT);
    if (!aborted) aborted = _wait(finalPauseMs);

    if (aborted) {
        _motors->stop();
        _comm->sendLog(F("Maniobra abortada por el control remoto, motores detenidos"));
    }
    // Descarta comandos viejos acumulados durante la maniobra (sobre todo con "U": Python no se bloquea)
    _comm->flushInput();
    _comm->sendStatusDone();
}

// Espera ms consultando _shouldAbort() en cada vuelta. Cada consulta tarda ~20 ms (lee el canal del
// switch), asi que la espera puede pasarse hasta ~30 ms. Devuelve true si hay que abortar.
bool Maneuvers::_wait(unsigned long ms) {
    unsigned long start = millis();
    while (millis() - start < ms) {
        if (_shouldAbort()) return true;
    }
    return false;
}

// Sigue girando mientras el sensor trasero del lado indicado lea mas que el delantero.
// Lee los sensores antes de evaluar: las lecturas previas son de antes del giro inicial.
// Devuelve true si hay que abortar.
bool Maneuvers::_align(bool useRightSide, int leftSpeed, int rightSpeed) {
    unsigned long start = millis();
    _sensors->update();
    while (useRightSide ? _sensors->getRRM() > _sensors->getRFM()
                        : _sensors->getLRM() > _sensors->getLFM()) {
        if (_shouldAbort()) return true;
        if (millis() - start > ALIGN_TIMEOUT_MS) {
            _motors->stop();
            _comm->sendLog(F("[ADVERTENCIA] ! : Alineacion supero ALIGN_TIMEOUT_MS, se continua la secuencia"));
            break;
        }
        _motors->move(leftSpeed, rightSpeed);
        _sensors->update();
    }
    return false;
}
