#ifndef sensors_config_h
#define sensors_config_h

#include <Arduino.h>

// Variables de distancia minima a paredes 
const int LL = 25;                        // Distancia lateral
const int LD = 25;                        // Distancia delantera

/* Ultrasónicos  */
// conexiones de triger y echo Izquierdo trasero 
const int trig_it = 41;           
const int echo_it = 42;
// conexiones de triger y echo Izquierdo delantero
const int trig_id = 5;
const int echo_id = 4;
// conexiones de triger y echo delantero izquierdo
const int trig_fi = 19;
const int echo_fi = 20;
// conexiones de triger y echo delantero derecho
const int trig_fd = 8;
const int echo_fd = 40;
// conexiones de triger y echo derecho delantero
const int trig_dd = 21;
const int echo_dd = 22;
// conexiones de triger y echo derecho trasero
const int trig_dt = 23;
const int echo_dt = 24;

// variables para sensores ultrasonicos 
long MIT,MID,MFI,MFD,MDD,MDT; // Mediciones de sensores ultrasonicos en centimetros 
int  LIT,LID,LFI,LFD,LDD,LDT; // Valores logicos de lectura de sensores ultrasonicos

const int pulseInDelay = 30000;

#endif