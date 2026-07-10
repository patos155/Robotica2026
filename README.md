# Robot de Rescate — Documentación del Proyecto

## Tabla de contenidos
- [Descripción general](#descripción-general)
- [Arquitectura del sistema](#arquitectura-del-sistema)
- [Estructura del proyecto](#estructura-del-proyecto)
- [Protocolo de comunicación Serial](#protocolo-de-comunicación-serial)
- [Convenciones de código](#convenciones-de-código)
- [Guía de contribución](#guía-de-contribución)

---

## Descripción general

Robot autónomo de rescate diseñado para navegar laberintos con diferentes tipos de terreno (piso liso, arena, rocas, escaleras). Opera en dos modos seleccionables físicamente desde un control remoto RC:

- **Modo autónomo:** navegación guiada por RPLIDAR (Python/ROS2) con ultrasonidos como seguridad local en el Arduino.
- **Modo manual:** control directo por palancas del transmisor RC.

### Hardware principal

| Componente | Descripción |
|---|---|
| Arduino Mega 2560 | Microcontrolador principal — control de motores y sensores |
| RPLIDAR (Slamtec) | Escáner láser 360° — navegación autónoma desde Python |
| 6× HC-SR04 | Ultrasonidos perimetrales — seguridad local |
| 4× Motores DC | Locomoción — control por puente H + relevadores + PWM |
| Receptor RC (6 canales) | Control remoto manual |
| 2× Cámaras USB | Visualización — procesadas en Python con OpenCV |
| Ubuntu 22.04 + ROS2 | Sistema operativo y framework de navegación |

---

## Arquitectura del sistema

El sistema está dividido en dos capas que se comunican por Serial USB a 9600 baud:

```
┌─────────────────────────────────────────┐
│         Ubuntu / ROS2 (Python)          │
│                                         │
│  RPLIDAR ──► NavigationNode             │
│               └─► decide dirección      │
│                    └─► Serial: JSON     │
│                                         │
│  Cámaras USB ──► OpenCV (visualización) │
│  Log JSON    ──► registro por sesión    │
└──────────────────┬──────────────────────┘
                   │ Serial USB
┌──────────────────▼──────────────────────┐
│             Arduino Mega                │
│                                         │
│  RC Receiver  ──► RemoteControl         │
│  6× HC-SR04   ──► UltrasonicArray       │
│  Motores      ──► MotorController       │
│  Maniobras    ──► Maneuvers             │
│  Serial       ──► SerialComm            │
└─────────────────────────────────────────┘
```

**Python** decide la navegación de alto nivel usando el LIDAR.
**Arduino** ejecuta las maniobras físicas y reacciona a obstáculos inmediatos con los ultrasonidos.

---

## Estructura del proyecto

```
robot_rescate/
│
├── README.md                      ← Este archivo
├── CONVENTIONS.md                 ← Convenciones y buenas prácticas del equipo
│
├── arduino/
│   └── robot_rescate/
│       ├── robot_rescate.ino      ← setup() y loop() — solo orquestación
│       ├── config.h               ← Pines, constantes y umbrales
│       ├── protocol.h             ← Contrato Serial Arduino ↔ Python
│       │
│       ├── motors/
│       │   ├── MotorController.h
│       │   └── MotorController.cpp
│       │
│       ├── sensors/
│       │   ├── UltrasonicArray.h
│       │   └── UltrasonicArray.cpp
│       │
│       ├── maneuvers/
│       │   ├── Maneuvers.h
│       │   └── Maneuvers.cpp
│       │
│       ├── communication/
│       │   ├── SerialComm.h
│       │   └── SerialComm.cpp
│       │
│       └── remote/
│           ├── RemoteControl.h
│           └── RemoteControl.cpp
│
└── python/
    └── obstacle_avoidance.py      ← Nodo ROS2 de navegación
```

### Responsabilidad de cada módulo Arduino

| Módulo | Responsabilidad |
|---|---|
| `robot_rescate.ino` | Punto de entrada. Solo inicializa y orquesta — sin lógica propia. |
| `config.h` | Único lugar donde viven pines, umbrales y tiempos. Si cambia el hardware, solo se toca aquí. |
| `protocol.h` | Define los tipos de mensaje, comandos y valores del Serial. Contrato entre Arduino y Python. |
| `MotorController` | Único módulo que escribe en los pines de motor. Expone `forward()`, `backward()`, `stop()`, `turnLeft()`, `turnRight()`, `setLeft()`, `setRight()`. |
| `UltrasonicArray` | Lee los 6 HC-SR04 y expone distancias en cm, flags de obstáculo y deltas de alineación. |
| `Maneuvers` | Ejecuta maniobras completas (giro izquierda, derecha, U) incluyendo ajuste fino post-giro. Usa `MotorController` y `UltrasonicArray`. |
| `SerialComm` | Toda la comunicación con Python: recibe comandos JSON, envía JSON de sensores y respuestas. |
| `RemoteControl` | Lee los 6 canales RC y determina el modo de operación. |

---

## Protocolo de comunicación Serial

Velocidad: **9600 baud**. Todos los mensajes terminan en `\n`.
Todos los mensajes son **JSON** con el campo `"type"` obligatorio.

### Catálogo de tipos

| `type` | `sensor` | Dirección | Descripción |
|---|---|---|---|
| `"cmd"` | — | Python → Arduino | Comando de movimiento |
| `"status"` | — | Arduino → Python | Estado de una maniobra |
| `"mode"` | — | Arduino → Python | Cambio de modo de operación |
| `"sensor"` | `"ultrasonic"` | Arduino → Python | Lecturas de los 6 HC-SR04 |
| `"sensor"` | `"gas"` | Arduino → Python | Lectura sensor de gas (pendiente) |
| `"sensor"` | `"humidity"` | Arduino → Python | Lectura sensor de humedad (pendiente) |
| `"lidar"` | — | Arduino → Python | Estado de conexión del LIDAR |
| `"log"` | — | Arduino → Python | Mensajes de debug — no afectan lógica |

### Python → Arduino

```json
{"type": "cmd", "value": "F"}
{"type": "cmd", "value": "L"}
{"type": "cmd", "value": "R"}
{"type": "cmd", "value": "U"}
{"type": "cmd", "value": "S"}
```

### Arduino → Python

```json
{"type": "status", "value": "DONE"}

{"type": "mode", "value": "AUTO"}
{"type": "mode", "value": "MANUAL"}

{"type": "sensor", "sensor": "ultrasonic", "data": {"distFL": 45, "distFR": 48, "distLF": 30, "distLR": 32, "distRF": 28, "distRR": 31}}
{"type": "sensor", "sensor": "gas",        "data": {"digital": 0, "analog": 312}}
{"type": "sensor", "sensor": "humidity",   "data": {"temp": 24.5, "humidity": 68}}

{"type": "lidar", "value": "CONNECTED"}
{"type": "lidar", "value": "LOST"}

{"type": "log",   "value": "Sistema iniciado"}
```

> **Nota:** Los sensores de gas y humedad están pendientes de hardware. La estructura del mensaje ya está definida para cuando se integren.

---

## Convenciones de código

Ver **[CONVENTIONS.md](./CONVENTIONS.md)** para la guía completa de convenciones y buenas prácticas del equipo.

---

## Guía de contribución

### Antes de hacer un cambio

1. Si el cambio afecta pines, umbrales o tiempos → editar solo `config.h`.
2. Si el cambio afecta el protocolo Serial → editar `protocol.h` y actualizar la tabla en este README.
3. Si se agrega un sensor nuevo → crear su módulo en `sensors/` y agregarlo a la tabla de módulos.

### Al agregar un sensor nuevo (gas, humedad, etc.)

```
sensors/
├── UltrasonicArray.h/.cpp   ← existente
├── GasSensor.h/.cpp         ← nuevo — misma estructura: begin() + update()
└── HumiditySensor.h/.cpp    ← nuevo — misma estructura: begin() + update()
```

Cada sensor nuevo debe:
- Tener sus pines definidos en `config.h`
- Exponer al menos `begin()` y `update()`
- Incluir su lectura en el JSON que envía `SerialComm`
- Documentar el formato del JSON en la sección de protocolo de este README

### Checklist antes de subir cambios

- [ ] Las constantes nuevas están en `config.h`, no en el archivo donde se usan
- [ ] Los nombres siguen las convenciones definidas en `CONVENTIONS.md`
- [ ] Los comentarios explican el por qué, no el qué
- [ ] `loop()` sigue siendo solo orquestación
- [ ] Solo `MotorController` escribe en pines de motor
- [ ] Solo `SerialComm` escribe en `Serial`
- [ ] El protocolo Serial no cambió sin actualizar este README
