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
}