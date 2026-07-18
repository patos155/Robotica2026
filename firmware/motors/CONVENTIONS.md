# Convenciones — Firmware Motors

Convenciones específicas de este firmware. Para convenciones de Git y flujo de trabajo del repositorio, ver `CONTRIBUTING.md` en la raíz. Para convenciones transversales al proyecto completo, ver `docs/CONVENTIONS.md`.

## Idioma

Código en inglés. Comentarios y documentación en español.

## Nomenclatura

| Elemento | Convención | Ejemplo |
|---|---|---|
| Variables, funciones | `camelCase` | `sensorDistance`, `readSensor()` |
| Clases | `PascalCase` | `MotorController` |
| Constantes, pines | `UPPER_SNAKE_CASE`, prefijo `PIN_` para pines | `PWM_FORWARD`, `PIN_TRIG_FL` |
| Booleanos | prefijo `is`/`has` | `isObstacleDetected` |
| Atributos privados | prefijo `_` | `_distFL` |

## Tipos de datos

- Ancho fijo (`uint8_t`, `uint16_t`, `uint32_t`) sobre `int` genérico cuando el tamaño importa.
- `bool` para flags lógicos — nunca `int`.
- `float` siempre con sufijo `f` (`0.35f`).
- `constexpr` siempre — nunca `#define` para constantes. Excepción: `DEBUG_MODE`, que requiere `#define` para que el preprocesador elimine código con `#ifdef`.

## Comentarios

Explican el **por qué**, no el qué. Encabezado breve en cada `.h` describiendo propósito y restricciones del módulo.

## Grafo de dependencias permitido

```
robot_rescate.ino
   ├── SerialComm      usa → Maneuvers, MotorController
   ├── RemoteControl   usa → MotorController
   ├── Maneuvers       usa → MotorController, UltrasonicArray
   └── UltrasonicArray, MotorController → sin dependencias del proyecto (solo config.h)
```

## Reglas de arquitectura (no negociables)

- Solo `MotorController` escribe en pines de motor.
- Solo `SerialComm` y `Logger` escriben en `Serial`.
- `config.h` es el único lugar con números de pin.
- `loop()` no contiene lógica de negocio — decisiones como "modo autónomo ignora joystick" viven en `robot_rescate.ino`, no dentro de `RemoteControl`.
- `RemoteControl` solo reporta estado del switch/joystick — nunca decide qué hacer con esa información.
- `delay()` solo permitido dentro de `Maneuvers`. Candidato a reemplazarse por `millis()` si superficies irregulares dan problemas de alineación.
- Inicialización de hardware en `begin()` llamado desde `setup()` — nunca en constructores.
- Dirección de motores vía `MOT_FORWARD`/`MOT_BACKWARD` de `config.h` — nunca `HIGH`/`LOW` hardcodeado.
- Lecturas de ultrasonido inválidas devuelven `-1` — siempre filtrar antes de usar.
- **Watchdog obligatorio:** función explícita `verificarWatchdog()` en `robot_rescate.ino`, llamada en cada `loop()`. Detiene motores si no llega comando nuevo en ~300–500ms.

## Logging

`Logger` con 4 niveles: `INFO`, `WARNING`, `ERROR`, `DEBUG`. Controlado por `#define DEBUG_MODE` en `config.h` — inactivo, solo se envían `WARNING`/`ERROR`. Strings siempre con macro `F()`.

```cpp
Logger::info(F("Sistema iniciado"));
Logger::debug(F("distFL="), ultrasonics.getDistFL());
```

## Protocolo Serial

JSON por línea, campo `"type"` obligatorio. Cualquier cambio a `protocol.h` debe reflejarse en el mismo PR en `docs/protocols/motors-protocol.md`.