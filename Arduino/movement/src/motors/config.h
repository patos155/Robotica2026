#ifndef motors_config_h
#define motors_config_h

#include <Arduino.h>

//pines para control de velocidad de arranque 
const int PWM_PINI = 6;
const int PWM_PIND = 7;

// definicion de salidas para control de los relevadores
const int left1=10;
const int left2=9;
const int right1=15;
const int right2=16;

long maxAutomaticPower = 1;
long automaticPower = 0.8;

// Potencias para motores 
const int speedLeftFront = 150;
const int speedRightFront = speedLeftFront;
const int speedLeftTurn = 200;
const int speedRightTurn = speedLeftTurn;
const int speedLeft = 0;
const int speedRight = 0;

#endif