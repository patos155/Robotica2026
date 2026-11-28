# Protocolo — `firmware/motors` ↔ `robot_ws`

**Estado:** definido y en uso.
**Medio:** Serial (USB), 9600 baud. Todos los mensajes terminan en `\n`.
**Formato:** JSON por línea, campo `"type"` obligatorio.

Fuente de verdad para ambos lados: si cambia algo aquí, debe cambiar en el mismo PR `firmware/motors/movement/protocol.h`, `firmware/motors/movement/config.h` y el nodo `serial_bridge` de `robot_ws`.

## Catálogo de tipos

| `type` | `sensor` | Dirección | Descripción |
|---|---|---|---|
| `cmd` | — | `robot_ws` → Arduino | Comando de movimiento |
| `status` | — | Arduino → `robot_ws` | Estado de una maniobra |
| `mode` | — | Arduino → `robot_ws` | Cambio de modo de operación |
| `sensor` | `ultrasonic` | Arduino → `robot_ws` | Lecturas de los 6 HC-SR04 |
| `log` | — | Arduino → `robot_ws` | Mensajes de debug — no afectan lógica |

> Los tipos `sensor`/`gas` y `sensor`/`humidity` que aparecían en el diseño original (cuando ambos sensores vivían en el mismo Arduino que los motores) se movieron a [`sensors-protocol.md`](./sensors-protocol.md) al separar en dos Arduinos. Confirmar que no quedó ninguna referencia residual en `firmware/motors/movement/protocol.h`.

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
- `sensor`/`ultrasonic`: distancias en cm de los 6 sensores. La nomenclatura es la de `UltrasonicArray.cpp`: `distFL`, `distFR`, `distLF`, `distLR`, `distRF`, `distRR` (F/L/R = frente/izquierda/derecha, la segunda letra indica el lado). `400` significa "sin eco o fuera de rango". El Arduino envía esta línea cada `SENSOR_TELEMETRY_MS` (200 ms, 5 Hz) porque a 9600 baud ocupa el loop ~140 ms.

## Modo legacy

Solo para probar el robot sin `serial_bridge`: con `LEGACY_PROTOCOL = true` en `firmware/motors/movement/config.h` el firmware habla texto plano como los scripts viejos. Cuando está activo:

- **Comandos:** la línea exacta `F`, `L`, `R`, `U` o `S`, sin JSON. Se tolera `\r\n`. Un comando desconocido detiene los motores.
- **Fin de maniobra:** `T` en vez de `{"type": "status", "value": "DONE"}`. El script viejo se bloquea tras `L`/`R` hasta recibirlo.
- **Logs:** texto plano de una línea con prefijo `LOG: `; nunca empiezan con `{` ni son exactamente `T`.
- **Modo:** `MODE: AUTO` o `MODE: MANUAL` en vez de `{"type": "mode", ...}`.
- **Sensores:** no se envían; a 9600 baud esa línea bloquea el loop entre 90 y 150 ms.
- **Watchdog:** desactivado, porque el script viejo manda `F` una sola vez y un watchdog de 500 ms detendría el robot a medio segundo de cada `F`.

Con `LEGACY_PROTOCOL = false` (valor actual) aplica el protocolo JSON de este documento: el watchdog de 500 ms está activo, por eso `serial_bridge` reenvía el comando vigente a 5 Hz, espera `status/DONE` tras `L`/`R`/`U`, y no envía nada en `MANUAL` (el control remoto manda en ese modo).

## Watchdog relacionado

Si `robot_ws` deja de mandar `cmd` por más de ~300-500ms mientras el Arduino está en modo autónomo, el Arduino debe detenerse por su cuenta (watchdog local, no depende de recibir un `cmd: S` explícito). Ver `docs/CONVENTIONS.md` → "Cuándo algo necesita un watchdog".