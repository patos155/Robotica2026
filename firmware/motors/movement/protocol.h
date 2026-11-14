#pragma once

namespace Cmd{
    // Definicion de comandos Seriales recibidos desde Python
    constexpr char cmdForward   = 'F';
    constexpr char cmdLeft      = 'L';
    constexpr char cmdRight     = 'R';
    constexpr char cmdUturn     = 'U';
    constexpr char cmdStop      = 'S';
}

// Definicion de estados enviados del Arduino
namespace Status {
    const char* const Done      = "DONE";
    const char* const DoneLegacy = "T";     // fin de maniobra con LEGACY_PROTOCOL (nagevacion_legacy.py)
}

// Valores del mensaje de modo (refleja el switch del control remoto)
namespace Mode {
    const char* const Auto      = "AUTO";
    const char* const Manual    = "MANUAL";
}