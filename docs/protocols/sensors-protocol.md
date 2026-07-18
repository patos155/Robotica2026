# Protocolo — `firmware/sensors` ↔ `robot_ws`

**Estado:** borrador — `firmware/sensors` aún no está implementado. Este documento es una propuesta inicial, no un contrato ya en uso. **Medio:** Serial (USB), 9600 baud (a confirmar si conviene otro valor).

A diferencia del protocolo de motores, este enlace tolera más latencia y puede darse el lujo de tramas más verbosas — no hay riesgo de choque si una lectura de gas llega 200ms tarde.

## Catálogo de tipos propuesto

| `type` | `sensor` | Dirección | Descripción |
|---|---|---|---|
| `cmd` | — | `robot_ws` → Arduino | Mover la cápsula a una posición (3 ejes) |
| `status` | — | Arduino → `robot_ws` | Cápsula llegó a posición / lectura lista |
| `sensor` | `gas` | Arduino → `robot_ws` | Lectura de gas |
| `sensor` | `env` | Arduino → `robot_ws` | Temperatura + humedad |
| `log` | — | Arduino → `robot_ws` | Mensajes de debug |

## `robot_ws` → Arduino (propuesto)

```json
{"type": "cmd", "action": "move_capsule", "x": 10, "y": 5, "z": 0}
```

Coordenadas en la unidad que use `RailController` internamente (mm o pasos de motor — **a definir cuando se implemente**).

## Arduino → `robot_ws` (propuesto)

```json
{"type": "status", "value": "POSITION_REACHED"}

{"type": "sensor", "sensor": "gas",    "data": {"digital": 0, "analog": 312}}
{"type": "sensor", "sensor": "env",    "data": {"temp": 24.5, "humidity": 68}}

{"type": "log", "value": "Sistema iniciado"}
```

## Decisiones pendientes antes de que este documento pase de "borrador" a "definido"

- [ ] Confirmar unidades y rango de movimiento de `RailController`
- [ ] Confirmar si 9600 baud es suficiente o conviene subirlo, dado que las tramas aquí pueden ser más grandes que las de motores