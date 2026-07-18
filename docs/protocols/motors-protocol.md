# Protocolo — `firmware/motors` ↔ `robot_ws`

**Estado:** definido y en uso.
**Medio:** Serial (USB), 9600 baud. Todos los mensajes terminan en `\n`.
**Formato:** JSON por línea, campo `"type"` obligatorio.

Fuente de verdad para ambos lados: si cambia algo aquí, debe cambiar en el mismo PR tanto `firmware/motors/src/protocol.h` como el nodo `motors_bridge` de `robot_ws`.

## Catálogo de tipos

| `type` | `sensor` | Dirección | Descripción |
|---|---|---|---|
| `cmd` | — | `robot_ws` → Arduino | Comando de movimiento |
| `status` | — | Arduino → `robot_ws` | Estado de una maniobra |
| `mode` | — | Arduino → `robot_ws` | Cambio de modo de operación |
| `sensor` | `ultrasonic` | Arduino → `robot_ws` | Lecturas de los 6 HC-SR04 |
| `log` | — | Arduino → `robot_ws` | Mensajes de debug — no afectan lógica |

> Los tipos `sensor`/`gas` y `sensor`/`humidity` que aparecían en el diseño original (cuando ambos sensores vivían en el mismo Arduino que los motores) se movieron a [`sensors-protocol.md`](./sensors-protocol.md) al separar en dos Arduinos. Confirmar que no quedó ninguna referencia residual en `firmware/motors/src/protocol.h`.

## `robot_ws` → Arduino

```json
{"type": "cmd", "value": "F"}
{"type": "cmd", "value": "L"}
{"type": "cmd", "value": "R"}
{"type": "cmd", "value": "U"}
{"type": "cmd", "value": "S"}
```

`F` avanzar, `L` giro izquierda, `R` giro derecha, `U` vuelta en U, `S` detener.

## Arduino → `robot_ws`

```json
{"type": "status", "value": "DONE"}

{"type": "mode", "value": "AUTO"}
{"type": "mode", "value": "MANUAL"}

{"type": "sensor", "sensor": "ultrasonic", "data": {"distFL": 45, "distFR": 48, "distLF": 30, "distLR": 32, "distRF": 28, "distRR": 31}}

{"type": "log", "value": "Sistema iniciado"}
```

- `status`/`DONE`: el Arduino terminó de ejecutar la maniobra en curso — `robot_ws` puede volver a mandar comandos.
- `mode`: refleja el estado físico del switch del control remoto, no una decisión de `robot_ws`.
- `sensor`/`ultrasonic`: distancias en cm de los 6 sensores. Nomenclatura: `dist` + posición (`F`ront/`L`eft/`R`ight/`D`erecha... — **confirmar convención exacta de nombres con quien mantenga `UltrasonicArray.cpp`**, aquí se replica tal cual del README original.

## Watchdog relacionado

Si `robot_ws` deja de mandar `cmd` por más de ~300-500ms mientras el Arduino está en modo autónomo, el Arduino debe detenerse por su cuenta (watchdog local, no depende de recibir un `cmd: S` explícito). Ver `docs/CONVENTIONS.md` → "Cuándo algo necesita un watchdog".