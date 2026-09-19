# Robot de Rescate — TMR 2027

Robot para **RoboCup Major – Rescue Robot** (Torneo Mexicano de Robótica).
Navega carriles con terrenos y obstáculos variados (rampas, grava, K-Rails,
stepfields, escaleras, puertas) en modo manual o autónomo, y resuelve las
tareas de sensado de la caja de víctima (imagen térmica, hazmat, movimiento,
polo de imán, QR y audio bidireccional). El reglamento de referencia es
RoboCup Rescue **2026D**, resumido por tema en [`docs/rules/`](./docs/rules),
hasta que se publique el del siguiente año.

## Componentes

| Componente | Qué hace | Se comunica por |
|---|---|---|
| `firmware/motors` | Motores, ultrasónicos, recepción RC | Serial con `robot_ws` |
| `firmware/sensors` | Sensores de la caja de víctima (hoy: polo de imán) | Serial con `robot_ws` |
| `robot_ws` | Navegación autónoma (ROS2 + LiDAR), agregación de datos | Serial con ambos firmwares, WiFi con `dashboard` |
| `dashboard` | Interfaz del operador — video, mapa, sensores, control | WiFi (rosbridge / HTTP) con `robot_ws` |

Regla de diseño: **lo crítico para la seguridad (motores, evasión de
obstáculos) nunca depende del WiFi hacia el operador** — el robot navega
solo con su laptop embarcada y los dos Arduinos, por cable. El WiFi solo
transporta lo que el operador ve y los comandos que manda.

Diagrama completo y razonamiento: [`docs/ARCHITECTURE.md`](./docs/ARCHITECTURE.md).

## Estructura

```
robot_rescate/
├── docs/            ← arquitectura, convenciones generales, protocolos, reglas de la competencia
├── firmware/
│   ├── motors/      ← Arduino: motores, ultrasónicos, RC
│   └── sensors/     ← Arduino: sensores de la caja de víctima
├── robot_ws/        ← workspace ROS2 (laptop del robot)
├── dashboard/       ← interfaz web del operador (Vue 3)
├── Python/          ← scripts sueltos de prueba de detección (QR, movimiento)
└── .devcontainer/   ← contenedor ROS2 Humble para robot_ws
```

Cada componente documenta su detalle en su propio `README.md` y
`CONVENTIONS.md`; todavía no existen en `firmware/sensors`, y `dashboard`
no tiene `CONVENTIONS.md`.

## Primeros pasos

| Componente | Comando |
|---|---|
| `firmware/sensors` | VS Code + PlatformIO → `pio run -t upload` |
| `firmware/motors` | Sketch `movement/movement.ino` (aún sin `platformio.ini`) |
| `robot_ws` | `colcon build && source install/setup.bash && ros2 launch robot_vision robot_vision.launch.py` (o *Reopen in Container* con `.devcontainer/robot_ws`) |
| `dashboard` | `npm install` (una vez) → `npm run dev`; **`npm run build` antes de competencia** |

## Documentación

- [`docs/ARCHITECTURE.md`](./docs/ARCHITECTURE.md) — diagrama y por qué cada enlace usa el medio que usa
- [`docs/CONVENTIONS.md`](./docs/CONVENTIONS.md) — reglas transversales, sin importar el lenguaje
- [`docs/protocols/`](./docs/protocols) — formato exacto de cada mensaje entre componentes
- [`docs/rules/`](./docs/rules) — reglas de RoboCup Rescue 2026D por tema (arena, destreza, caja de víctima, mapeo), con figuras y pendientes
- [`CONTRIBUTING.md`](./CONTRIBUTING.md) — ramas, commits, checklist de PR

## Estado

Al 2026-09-18, en la rama `Dev`.

**Definido y documentado:**
- Arquitectura general de los 4 componentes y sus 3 enlaces
- Convenciones transversales ([`docs/CONVENTIONS.md`](./docs/CONVENTIONS.md)), modelo de ramas, commits y checklist de PR
- Protocolo Serial `firmware/motors` ↔ `robot_ws`: definido e implementado del lado del firmware ([`motors-protocol.md`](./docs/protocols/motors-protocol.md)); falta el puente en `robot_ws`
- Estructura de módulos y convenciones de `firmware/motors`, con watchdog de 500 ms en el código
- Paquetes de `robot_ws`: `robot_interfaces` (Action `RunTest`) y `robot_vision`, con devcontainer ROS2 Humble
- `dashboard` en Vue 3 + Vite, conectado a `robot_ws` por rosbridge (WebSocket) y `web_video_server`
- La decodificación de QR corre en `robot_ws` (`pyzbar`), no en un Arduino ni en el `dashboard`: el reglamento exige que sea autónoma
- Reglamento de referencia (RoboCup Rescue 2026D) resumido por tema en `docs/rules/`; sus pendientes están en [`PENDIENTES.md`](./docs/rules/PENDIENTES.md)

**Pendiente de definir:**
- [ ] Cerrar el borrador de [`telemetry-protocol.md`](./docs/protocols/telemetry-protocol.md): tipo de mensaje de `/robot/pose` y `/robot/sensors`, JSON de `/robot/sensors`, `/robot/emergency_stop` como servicio o topic, y timeout del heartbeat
- [ ] Poner al día [`sensors-protocol.md`](./docs/protocols/sensors-protocol.md): propone gas, temperatura y humedad, pero solo el imán está implementado y el gas se confirmó como no aplicable
- [ ] Estructura interna de `dashboard/` a detalle (componentes, composables)
- [ ] Brazo vs. riel para las tareas de destreza (decisión de mecánica): bloquea el software de manipulación
- [ ] Modelo de placa física de los dos Arduinos (`firmware/sensors` compila con `uno` como placeholder)
- [ ] Pendientes del reglamento: ver [`docs/rules/PENDIENTES.md`](./docs/rules/PENDIENTES.md)

**Pendiente de construir:**
- [ ] `firmware/motors` — hacer que compile (carácter suelto `z` en `config.h:22`, `System dbg;` sin tipo `System` en `movement.ino`, `#include` sin `src/`), migrar a PlatformIO y poner al día su `CONVENTIONS.md` (aún menciona `robot_rescate.ino` y `verificarWatchdog()`)
- [ ] `firmware/sensors` — solo tiene `MagnetSensor` (lectura del polo bajo demanda con `read_sensors`); falta su README/CONVENTIONS y definir si hace falta otro sensor para la caja de víctima
- [ ] `robot_ws` — ya tiene visión (cámara, compresión, pruebas QR y movimiento bajo `/run_test`); falta:
  - handlers de `test_runner` para hazmat y audio bidireccional
  - `robot_bringup` / `robot_core`: navegación, LiDAR + SLAM, agregación de datos
  - puente Serial con ambos firmwares
  - exportación de mapas GeoTIFF/PLY/CSV ([`docs/rules/05-mapeo.md`](./docs/rules/05-mapeo.md))
  - una exploración previa de navegación con LiDAR (scripts sueltos, sin portar a ROS2) existe solo en `robot_ws/legacy/` de las ramas `feat/codigo-sensores` y `dashboardCamRos2`
- [ ] `dashboard` — hay un prototipo Vue 3 (video en vivo, prueba QR, conexión rosbridge con reconexión y banner de sin comunicación); falta mapa, sensores, control y botones para las demás pruebas
- [ ] Integración de punta a punta probada sobre el robot físico

## Contribuir

Ver [`CONTRIBUTING.md`](./CONTRIBUTING.md) antes del primer cambio.
