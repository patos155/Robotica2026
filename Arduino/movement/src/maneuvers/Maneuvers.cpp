#include "Maneuvers.h"
#include "config.h"

void Maneuvers::begin(Motors * motorsInstance, UltrasonicArray * sensorsInstance) {
    _motors = motorsInstance;
    _sensors = sensorsInstance;
}

void Maneuvers::update() {
    if (_sensors->isFLL() == 0 && _sensors->isFRL() == 0) {
        _motors->stop();
    }
}

// Giro hacia la derecha
void Maneuvers::turnRight() {
    _motors->stop(); delay(delayStop);
    _motors->move(speedLeftTurn, -speedRightTurn); delay(delayTurn);
    _motors->stop(); delay(delayStop);
    _sensors->update();
    while (_sensors->getLFM() > _sensors->getLRM()) {
        _motors->move(speedLeftTurn, -speedRightTurn);
        _sensors->update();
    }
    _motors->stop(); delay(delayStop);
    _motors->move(speedLeftFront, speedRightFront); delay(delayForward);
}

// Giro hacia la izquierda
void Maneuvers::turnLeft(){
    _motors->stop(); delay(delayStop);
    _motors->move(speedRightTurn, -speedLeftTurn); delay(delayTurn);
    _motors->stop(); delay(delayStop);
    while (_sensors->getRFM() > _sensors->getRRM()) {
        _motors->move(-speedLeftTurn, speedRightTurn);
        _sensors->update();
    }
    _motors->stop(); delay(delayStop);
    _motors->move(speedLeftFront, speedRightFront); delay(delayForward);
}

// Vuelta en U
void Maneuvers::uTurn(){
    _motors->stop(); delay(delayStop);
    _motors->move(speedRightTurn, -speedLeftTurn); delay(delayUTurn);
    _motors->stop(); delay(delayStop);
    while (_sensors->getLFM() > _sensors->getLRM()){
        _motors->move(-speedLeftTurn, speedRightTurn);
        _sensors->update();
    }
    _motors->stop(); delay(delayStop);
    _motors->move(speedLeftFront, speedRightFront); delay(delayForward);
}