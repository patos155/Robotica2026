#include "UltrasonicArray.h"
#include "config.h"

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
    // Realizar las mediciones fisicas en centimetros
    _lrm = _readUltra(PIN_LEFT_REAR_TRIGGER, PIN_LEFT_REAR_ECHO);
    delay(5);
    _lfm = _readUltra(PIN_LEFT_FRONT_TRIGGER, PIN_LEFT_FRONT_ECHO);
    delay(5);
    _flm = _readUltra(PIN_FRONT_LEFT_TRIGGER, PIN_FRONT_LEFT_ECHO);
    delay(5);
    _frm = _readUltra(PIN_FRONT_RIGHT_TRIGGER, PIN_FRONT_RIGHT_ECHO);
    delay(5);
    _rfm = _readUltra(PIN_RIGHT_FRONT_TRIGGER, PIN_RIGHT_FRONT_ECHO);
    delay(5);
    _rrm = _readUltra(PIN_RIGHT_REAR_TRIGGER, PIN_RIGHT_REAR_ECHO);

    // Devuelve "true" si el camino esta libre, false si hay obstaculo
    _lrl = (_lrm > lateralDistance) ? 1 : 0;
    _lfl = (_lfm > lateralDistance) ? 1 : 0;
    _fll = (_flm > frontDistance) ? 1 : 0;
    _frl = (_frm > frontDistance) ? 1 : 0;
    _rfl = (_rfm > lateralDistance) ? 1 : 0;
    _rrl = (_rrm > lateralDistance) ? 1 : 0;

}

long UltrasonicArray::_readUltra(int trigPin, int echoPin) {
    // Generar el pulso de disparo (Trigger)
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, pulseInDelay);
    // Retornamos una distancia alta (200 cm) para indicar que el camino esta despejado
    if (duration == 0 || duration > 2500) {
        return -1; 
    }
    
    // Convertir el tiempo en distancia (cm)
    return duration / 59;
}