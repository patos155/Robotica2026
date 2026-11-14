#include "Maneuvers.h"
#include "../../config.h"

void Maneuvers::begin(Motors * motorsInstance, UltrasonicArray * sensorsInstance, Communication* comm) {
    _motors = motorsInstance;
    _sensors = sensorsInstance;
    _comm = comm;
}

// Giro hacia la derecha (legacy: vueltaDerecha)
void Maneuvers::turnRight() {
    _motors->stop(); delay(DELAY_STOP);
    _motors->move(SPEED_LEFT_TURN, -SPEED_RIGHT_TURN); delay(RIGHT_TURN_MS);
    _motors->stop(); delay(DELAY_STOP);
    _align(false, SPEED_LEFT_TURN, -SPEED_RIGHT_TURN);      // while (LRM > LFM)
    _finish(RIGHT_FINAL_PAUSE_MS);
}

// Giro hacia la izquierda (legacy: vueltaIzquierda)
void Maneuvers::turnLeft(){
    _motors->stop(); delay(DELAY_STOP);
    _motors->move(-SPEED_LEFT_TURN, SPEED_RIGHT_TURN); delay(LEFT_TURN_MS);
    _motors->stop(); delay(DELAY_STOP);
    _align(true, -SPEED_LEFT_TURN, SPEED_RIGHT_TURN);       // while (RRM > RFM)
    _finish(LEFT_FINAL_PAUSE_MS);
}

// Vuelta en U, girando a la izquierda (legacy: vueltaU)
void Maneuvers::uTurn(){
    _motors->stop(); delay(DELAY_STOP);
    _motors->move(-SPEED_LEFT_TURN, SPEED_RIGHT_TURN); delay(U_TURN_MS);
    _motors->stop(); delay(DELAY_STOP);
    _align(true, -SPEED_LEFT_TURN, SPEED_RIGHT_TURN);       // while (RRM > RFM)
    _finish(U_FINAL_PAUSE_MS);
}

// Sigue girando mientras el sensor trasero del lado indicado lea mas que el delantero.
// Lee los sensores antes de evaluar: las lecturas previas son de antes del giro inicial.
void Maneuvers::_align(bool useRightSide, int leftSpeed, int rightSpeed) {
    unsigned long start = millis();
    _sensors->update();
    while (useRightSide ? _sensors->getRRM() > _sensors->getRFM()
                        : _sensors->getLRM() > _sensors->getLFM()) {
        if (millis() - start > ALIGN_TIMEOUT_MS) {
            _motors->stop();
            _comm->sendLog(F("[ADVERTENCIA] ! : Alineacion supero ALIGN_TIMEOUT_MS, se continua la secuencia"));
            break;
        }
        _motors->move(leftSpeed, rightSpeed);
        _sensors->update();
    }
}

void Maneuvers::_finish(int finalPauseMs) {
    _motors->stop(); delay(DELAY_STOP);
    // Legacy ponia los 4 reles en HIGH: en un medio puente de reles eso frena el motor, asi que en
    // la practica es una pausa. Despues Python manda "F" por su cuenta. Si en el robot se confirma
    // que legacy avanzaba en este paso, basta cambiar esta linea por
    // _motors->move(SPEED_LEFT_FRONT, SPEED_RIGHT_FRONT);
    _motors->stop(); delay(finalPauseMs);
    // Descarta comandos viejos acumulados durante la maniobra (sobre todo con "U": Python no se bloquea)
    _comm->flushInput();
    _comm->sendStatusDone();
}
