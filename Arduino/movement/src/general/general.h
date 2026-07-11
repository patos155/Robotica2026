#pragma once
#include <Arduino.h>

// Función para mapear un valor de un rango a otro rango
constexpr float fmap(float x, float inMin, float inMax, float outMin, float outMax)
{
  // Convierte el valor 'x' del rango [in_min, in_max] al rango [out_min, out_max]
  return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}