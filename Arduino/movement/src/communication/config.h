#ifndef communication_config_h
#define communication_config_h

#include <Arduino.h>

// control remoto 
const int rcPins[6] = {25,29,27,31,33,35}

const float chValue[6];

// Función para mapear un valor de un rango a otro rango
const float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  // Convierte el valor 'x' del rango [in_min, in_max] al rango [out_min, out_max]
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif