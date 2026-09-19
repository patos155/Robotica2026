#pragma once

// =============================================================
// protocol.h
// Contrato Serial con robot_ws — un JSON por línea, campo "type"
// obligatorio. Cambios aquí van también en
// docs/protocols/sensors-protocol.md (mismo PR).
//
// Alcance actual: solo magnet. gas/env se agregan cuando
// se retomen esos sensores.
// =============================================================

namespace Protocol {
    namespace Type {
        constexpr const char* CMD    = "cmd";
        constexpr const char* SENSOR = "sensor";
        constexpr const char* LOG    = "log";
    }

    namespace Action {
        constexpr const char* READ_SENSORS = "read_sensors";
    }

    namespace Sensor {
        constexpr const char* MAGNET = "magnet";
    }

    // Nombres de campos JSON. Centralizados para no repetir strings
    // sueltos — un typo en una clave rompe el contrato en silencio.
    namespace Key {
        constexpr const char* TYPE     = "type";
        constexpr const char* ACTION   = "action";
        constexpr const char* SENSOR   = "sensor";
        constexpr const char* DATA     = "data";
        constexpr const char* VALUE    = "value";
        constexpr const char* DETECTED = "detected";
        constexpr const char* POLE     = "pole";
    }

    namespace Pole {
        constexpr const char* NORTH = "N";
        constexpr const char* SOUTH = "S";
        constexpr const char* NONE  = "NONE";
    }

    constexpr size_t JSON_DOC_CAPACITY = 128; // ajustar si se agregan campos
}  // namespace Protocol
