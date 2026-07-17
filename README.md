# Robot de Rescate — TMR 2027

Robot para **RoboCup Major – Rescue Robot** (Torneo Mexicano de Robótica).
Navega pistas con terreno variado (piso liso, arena, rocas, escaleras) en
modo manual o autónomo, y realiza lecturas ambientales (gas, temperatura,
humedad, sonido, polo de imán, QR) en puntos específicos de la pista.

## Componentes

| Componente | Qué hace | Se comunica por |
|---|---|---|
| `firmware/motors` | Motores, ultrasónicos, recepción RC | Serial con `robot_ws` |
| `firmware/sensors` | Lecturas ambientales | Serial con `robot_ws` |
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
├── docs/            ← arquitectura, convenciones generales, protocolos
├── firmware/
│   ├── motors/      ← Arduino: motores, ultrasónicos, RC
│   └── sensors/     ← Arduino: sensado ambiental
├── robot_ws/         ← workspace ROS2 (laptop del robot)
├── dashboard/         ← interfaz web del operador (React)
└── scripts/           ← verificación del repo completo
```

Cada carpeta de primer nivel tiene su propio `README.md` y `CONVENTIONS.md`.

## Primeros pasos

| Componente | Comando |
|---|---|
| `firmware/motors` / `firmware/sensors` | VS Code + PlatformIO → `pio run -t upload` |
| `robot_ws` | `colcon build && source install/setup.bash && ros2 launch robot_bringup robot.launch.py` |
| `dashboard` | `npm install` (una vez) → `npm run dev`; **`npm run build` antes de competencia** |

## Documentación

- [`docs/ARCHITECTURE.md`](./docs/ARCHITECTURE.md) — diagrama y por qué cada enlace usa el medio que usa
- [`docs/CONVENTIONS.md`](./docs/CONVENTIONS.md) — reglas transversales, sin importar el lenguaje
- [`docs/protocols/`](./docs/protocols) — formato exacto de cada mensaje entre componentes
- [`CONTRIBUTING.md`](./CONTRIBUTING.md) — ramas, commits, checklist de PR

## Estado

**Definido y documentado:**
- Arquitectura general de los 4 componentes y sus 3 enlaces
- Estructura y convenciones de `firmware/motors` y `firmware/sensors`
- Estructura de paquetes de `robot_ws` (`robot_bringup`, `robot_core`, `robot_vision`)
- Modelo de ramas, commits y checklist de PR

**Pendiente de definir:**
- [ ] Dónde vive `qr_reader` — ¿Arduino con módulo dedicado o laptop con OpenCV?
- [ ] Contenido de `docs/protocols/telemetry-protocol.md`
- [ ] Contenido de `docs/CONVENTIONS.md` (transversal)
- [ ] Estructura interna de `dashboard/` a detalle (hooks, componentes)

**Pendiente de construir:**
- [ ] `firmware/motors` — en refactor sobre la estructura ya definida
- [ ] `firmware/sensors` — sin implementar
- [ ] `robot_ws` — existe un nodo de navegación preliminar (ROS2) sin dividir en los paquetes definidos, sin probar en competencia
- [ ] `dashboard` — no iniciado
- [ ] Integración de punta a punta probada sobre el robot físico

## Contribuir

Ver [`CONTRIBUTING.md`](./CONTRIBUTING.md) antes del primer cambio.
