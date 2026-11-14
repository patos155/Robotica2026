# Convenciones — Firmware Motors

Convenciones específicas de este firmware. Para convenciones de Git y flujo de trabajo del repositorio, ver `CONTRIBUTING.md` en la raíz. Para convenciones transversales al proyecto completo, ver `docs/CONVENTIONS.md`.

## Idioma

Código en inglés. Comentarios y documentación en español.

## Nomenclatura

| Elemento | Convención | Ejemplo |
|---|---|---|
| Variables, funciones | `camelCase` | `sensorDistance`, `readSensor()` |
| Clases | `PascalCase` | `Motors` |
| Constantes, pines | `UPPER_SNAKE_CASE`, prefijo `PIN_` para pines | `CMD_TIMEOUT_MS`, `PIN_PWM_LEFT` |
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
movement.ino
   ├── Maneuvers       usa → Motors, UltrasonicArray, Communication
   │                   recibe → función de aborto bool (*shouldAbort)() definida en movement.ino
   ├── Logger          usa → Communication, Remote, UltrasonicArray
   ├── Communication   usa → UltrasonicArray
   └── Motors, UltrasonicArray, Remote → sin dependencias del proyecto (solo config.h)
```

`Maneuvers` no incluye `Remote`: para abortar una maniobra con el control remoto recibe en `begin()` un puntero a función (`shouldAbortManeuver()` en `movement.ino`).

## Reglas de arquitectura (no negociables)

- Solo `Motors` escribe en pines de motor.
- Solo `Communication` escribe en `Serial` (`Logger` lo hace a través de `Communication`).
- `config.h` es el único lugar con números de pin.
- `loop()` no contiene lógica de negocio — decisiones como "modo autónomo ignora joystick" viven en `movement.ino`, no dentro de `Remote`.
- `Remote` solo reporta estado del switch/joystick — nunca decide qué hacer con esa información.
- Esperas largas solo dentro de `Maneuvers`, con `_wait()`, que consulta la función de aborto en cada vuelta. `delay()` solo para pausas cortas de hardware, documentadas en `config.h`: `RELAY_SWITCH_MS` en `Motors` (los relés asientan antes de aplicar PWM) y `ULTRASONIC_GAP_MS` en `UltrasonicArray` (un sensor no lee el eco del anterior).
- Inicialización de hardware en `begin()` llamado desde `setup()` — nunca en constructores.
- Relés vía `RELAY_ON`/`RELAY_OFF` de `config.h` (niveles para energizar un relé, no direcciones) — nunca `HIGH`/`LOW` hardcodeado. Los relés nunca se conmutan con PWM aplicado.
- Lecturas de ultrasonido sin eco o fuera de rango devuelven `MAX_DISTANCE_CM` de `config.h` ("despejado"), como legacy: un HC-SR04 sin objeto cerca no devuelve eco dentro del timeout.
- **Watchdog obligatorio:** función explícita `checkWatchdog()` en `movement.ino`, llamada en cada `loop()` del modo autónomo. Detiene motores si no llega comando nuevo en `CMD_TIMEOUT_MS` (500 ms). Excepción: desactivado con `LEGACY_PROTOCOL = true`, porque `nagevacion_legacy.py` manda `F` una sola vez (ver `config.h`).

## Logging

`Logger` con 4 niveles: `INFO`, `WARNING`, `ERROR`, `DEBUG`. Controlado por `#define DEBUG_MODE` en `config.h` — inactivo, solo se envían `WARNING`/`ERROR`. Strings siempre con macro `F()`.

```cpp
dbg.info(F("Sistema iniciado"));                      // dbg: instancia de Logger en movement.ino
dbg.debug(F("distFL="), (int)ultraSensors.getFLM());
```

## Protocolo Serial

JSON por línea, campo `"type"` obligatorio. Excepción: con `LEGACY_PROTOCOL = true` se habla texto plano para `nagevacion_legacy.py` (ver "Modo legacy" en `docs/protocols/motors-protocol.md`). Cualquier cambio a `protocol.h` debe reflejarse en el mismo PR en `docs/protocols/motors-protocol.md`.