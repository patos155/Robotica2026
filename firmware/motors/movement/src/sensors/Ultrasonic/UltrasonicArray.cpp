#include "UltrasonicArray.h"
#include "../../../config.h"

void UltrasonicArray::begin() {
    // Configuracion de pines del lado izquierdo
    pinMode(PIN_LEFT_REAR_TRIGGER, OUTPUT); 
    pinMode(PIN_LEFT_REAR_ECHO, INPUT);

    pinMode(PIN_LEFT_FRONT_TRIGGER, OUTPUT);
    pinMode(PIN_LEFT_FRONT_ECHO, INPUT);

    // Configuracion de  pines del frente
    pinMode(PIN_FRONT_LEFT_TRIGGER, OUTPUT);
    pinMode(PIN_FRONT_LEFT_ECHO, INPUT);

    pinMode(PIN_FRONT_RIGHT_TRIGGER, OUTPUT);
    pinMode(PIN_FRONT_RIGHT_ECHO, INPUT);

    // Configuracion de pines del lado derecho
    pinMode(PIN_RIGHT_FRONT_TRIGGER, OUTPUT);
    pinMode(PIN_RIGHT_FRONT_ECHO, INPUT);

    pinMode(PIN_RIGHT_REAR_TRIGGER, OUTPUT);
    pinMode(PIN_RIGHT_REAR_ECHO, INPUT);
}

void UltrasonicArray::update() {
    // Realizar las mediciones fisicas en centimetros. La pausa entre disparos evita que un sensor
    // lea el eco del anterior
    _lrm = _readUltra(PIN_LEFT_REAR_TRIGGER, PIN_LEFT_REAR_ECHO);
    delay(ULTRASONIC_GAP_MS);
    _lfm = _readUltra(PIN_LEFT_FRONT_TRIGGER, PIN_LEFT_FRONT_ECHO);
    delay(ULTRASONIC_GAP_MS);
    _flm = _readUltra(PIN_FRONT_LEFT_TRIGGER, PIN_FRONT_LEFT_ECHO);
    delay(ULTRASONIC_GAP_MS);
    _frm = _readUltra(PIN_FRONT_RIGHT_TRIGGER, PIN_FRONT_RIGHT_ECHO);
    delay(ULTRASONIC_GAP_MS);
    _rfm = _readUltra(PIN_RIGHT_FRONT_TRIGGER, PIN_RIGHT_FRONT_ECHO);
    delay(ULTRASONIC_GAP_MS);
    _rrm = _readUltra(PIN_RIGHT_REAR_TRIGGER, PIN_RIGHT_REAR_ECHO);

    // Devuelve "true" si el camino esta libre, false si hay obstaculo
    _lrl = _lrm > LATERAL_DISTANCE;
    _lfl = _lfm > LATERAL_DISTANCE;
    _fll = _flm > FRONT_DISTANCE;
    _frl = _frm > FRONT_DISTANCE;
    _rfl = _rfm > LATERAL_DISTANCE;
    _rrl = _rrm > LATERAL_DISTANCE;

}

long UltrasonicArray::_readUltra(int trigPin, int echoPin) {
    // Generar el pulso de disparo (Trigger)
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, PULSE_IN_TIMEOUT_US);
    // Sin objeto cerca el HC-SR04 no regresa eco dentro del timeout (pulseIn da 0): se reporta
    // MAX_DISTANCE_CM ("despejado"), igual que legacy, que lo leia como una distancia grande
    if (duration == 0) return MAX_DISTANCE_CM;

    // Convertir el tiempo en distancia (cm)
    long distance = duration / 59;
    return (distance > MAX_DISTANCE_CM) ? MAX_DISTANCE_CM : distance;
}