#pragma once

#include <Arduino.h>

// control remoto 
constexpr int PINS_REMOTE_CONTROL[6] = {25,29,27,31,33,35}

constexpr float channelValue[6];

// Función para mapear un valor de un rango a otro rango
constexpr float fmap(float x, float inMin, float inMax, float outMin, float outMax)
{
  // Convierte el valor 'x' del rango [in_min, in_max] al rango [out_min, out_max]
  return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}