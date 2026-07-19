#include "../maneuvers/Maneuvers.h"
#include "config.h"

void Maneuvers::begin(Motors * motorsInstance, UltrasonicArray * sensorsInstance, Communication* comm) {
    _motors = motorsInstance;
    _sensors = sensorsInstance;
    _comm = comm;
}

// Giro hacia la derecha
void Maneuvers::turnRight() {
    _motors->stop(); delay(DELAY_STOP);
    _motors->move(SPEED_LEFT_TURN, -SPEED_RIGHT_TURN); delay(DELAY_TURN);
    _motors->stop(); delay(DELAY_STOP);
    _sensors->update();
    while (_sensors->getLFM() > _sensors->getLRM()) {
        _motors->move(SPEED_LEFT_TURN, -SPEED_RIGHT_TURN);
        _sensors->update();
    }
    _motors->stop(); delay(DELAY_STOP);
    _motors->move(SPEED_LEFT_FRONT, SPEED_RIGHT_FRONT); delay(DELAY_FORWARD);
    _comm->sendStatusDone();
}

// Giro hacia la izquierda
void Maneuvers::turnLeft(){
    _motors->stop(); delay(DELAY_STOP);
    _motors->move(SPEED_RIGHT_TURN, -SPEED_LEFT_TURN); delay(DELAY_TURN);
    _motors->stop(); delay(DELAY_STOP);
    while (_sensors->getRFM() > _sensors->getRRM()) {
        _motors->move(-SPEED_LEFT_TURN, SPEED_RIGHT_TURN);
        _sensors->update();
    }
    _motors->stop(); delay(DELAY_STOP);
    _motors->move(SPEED_LEFT_FRONT, SPEED_RIGHT_FRONT); delay(DELAY_FORWARD);
    _comm->sendStatusDone();
}

// Vuelta en U
void Maneuvers::uTurn(){
    _motors->stop(); delay(DELAY_STOP);
    _motors->move(SPEED_RIGHT_TURN, -SPEED_LEFT_TURN); delay(DELAY_U_TURN);
    _motors->stop(); delay(DELAY_STOP);
    while (_sensors->getLFM() > _sensors->getLRM()){
        _motors->move(-SPEED_LEFT_TURN, SPEED_RIGHT_TURN);
        _sensors->update();
    }
    _motors->stop(); delay(DELAY_STOP);
    _motors->move(SPEED_LEFT_FRONT, SPEED_RIGHT_FRONT); delay(DELAY_FORWARD);
    _comm->sendStatusDone();
}