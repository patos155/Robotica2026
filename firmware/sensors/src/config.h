#pragma once

#include <Arduino.h>

// =============================================================
// config.h
// Único archivo con pines y constantes de calibración (ver
// CONVENTIONS.md). Valores "A CONFIRMAR" son provisionales.
// =============================================================

#define DEBUG_MODE // activa logs INFO/DEBUG (ver CONVENTIONS.md)

constexpr uint32_t SERIAL_BAUD_RATE = 9600; // A CONFIRMAR (ver sensors-protocol.md)

// Sensor Hall lineal SS49E (detección de imán)
constexpr uint8_t PIN_MAGNET_ANALOG = A0;

// SS49E es ratiométrico: el reposo (sin campo) cae en la mitad del
// rango del ADC (~512 en 10 bits) sin importar el VCC exacto, porque
// sensor y ADC escalan juntos con la misma alimentación.
constexpr int16_t MAGNET_QUIESCENT_ADC = 512; // A CONFIRMAR con pruebas

// Zona muerta alrededor del reposo para ignorar ruido eléctrico.
constexpr int16_t MAGNET_DEADBAND_ADC = 15; // A CONFIRMAR con pruebas

// true si ADC > reposo = polo norte. El datasheet no es consistente
// entre fuentes — confirmar con un imán real y ajustar si sale al revés.
constexpr bool MAGNET_ABOVE_QUIESCENT_IS_NORTH = true; // A CONFIRMAR
