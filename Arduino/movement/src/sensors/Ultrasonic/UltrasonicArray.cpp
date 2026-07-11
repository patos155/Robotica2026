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
};

void UltrasonicArray::update() {
    // Realizar las mediciones fisicas en centimetros
    _lrm = readUltra(PIN_LEFT_REAR_TRIGGER, PIN_LEFT_REAR_ECHO);
    delay(5);
    _lfm = readUltra(PIN_LEFT_FRONT_TRIGGER, PIN_LEFT_FRONT_ECHO);
    delay(5);
    _flm = readUltra(PIN_FRONT_LEFT_TRIGGER, PIN_FRONT_LEFT_ECHO);
    delay(5);
    _frm = readUltra(PIN_FRONT_RIGHT_TRIGGER, PIN_FRONT_RIGHT_ECHO);
    delay(5);
    _rfm = readUltra(PIN_RIGHT_FRONT_TRIGGER, PIN_RIGHT_FRONT_ECHO);
    delay(5);
    _rrm = readUltra(PIN_RIGHT_REAR_TRIGGER, PIN_RIGHT_REAR_ECHO);

    // Devuelve "true" si el camino esta libre, false si hay obstaculo
    _lrl = (_lrm > lateralDistance);
    _lfl = (_lfm > lateralDistance);
    _fll = (_flm > frontDistance);
    _frl = (_frm > frontDistance);
    _rfl = (_rfm > lateralDistance);
    _rrl = (_rrm > lateralDistance);

};

long UltrasonicArray::_readUltra(int trigPin, int echoPin) {
    // Generar el pulso de disparo (Trigger)
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH, PULSE_IN_DELAY);
    
    // Si la lectura es 0 significa que se supero el tiempo de espera (Timeout)
    // Retornamos una distancia alta (999 cm) para indicar que el camino esta despejado
    if (duration == 0) {
        return 999; 
    }
    
    // Convertir el tiempo en distancia (cm)
    return duration / 59;
};