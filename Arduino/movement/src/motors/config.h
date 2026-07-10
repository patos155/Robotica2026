#pragma once
#include <Arduino.h>

//pines para control de velocidad de arranque 
constexpr int PIN_PWM_LEFT = 6;
constexpr int PIN_PWM_RIGHT = 7;

// definicion de salidas para control de los relevadores
constexpr int left1=10;
constexpr int left2=9;
constexpr int right1=15;
constexpr int right2=16;

constexpr long maxAutomaticPower = 1;
constexpr long automaticPower = 0.8;

// Potencias para motores 
constexpr int speedLeftFront = 150;
constexpr int speedRightFront = speedLeftFront;
constexpr int speedLeftTurn = 200;
constexpr int speedRightTurn = speedLeftTurn;
constexpr int speedLeft = 0;
constexpr int speedRight = 0;