# robot_ws

Workspace ROS2 que corre en la laptop embarcada del robot. Parte del proyecto
**Robot de Rescate TMR 2027**.

## Rol dentro del sistema

Por ahora este workspace solo cubre **visión** (cámara + detección de QR).
La navegación autónoma (LiDAR, SLAM, agregación de sensores) y el puente
Serial con `firmware/motors`/`firmware/sensors` descritos en
[`docs/ARCHITECTURE.md`](../docs/ARCHITECTURE.md) todavía no están
implementados aquí — hay una exploración previa sin portar en
[`robot_ws_legacy/`](../robot_ws_legacy) (scripts sueltos, no nodos ROS2,
nunca probados en competencia).

## Estructura

```
robot_ws/
├── src/
│   └── robot_vision/              ← paquete ament_python
│       ├── package.xml
│       ├── setup.py / setup.cfg
│       ├── resource/
│       ├── robot_vision/
│       │   ├── camera_publisher.py     ← nodo: abre la cámara USB, publica /camera/image_raw
│       │   └── detectors/
│       │       └── qr_detector.py      ← nodo: suscribe a /camera/image_raw, decodifica QR (pyzbar), publica /qr_test/image_processed
│       └── test/                        ← linters ament (copyright, flake8, pep257)
├── build/ install/ log/            ← generados por colcon, ignorados por git
```

`robot_bringup` y `robot_core` (navegación, agregación de datos) descritos en
el README raíz todavía no existen como paquetes — ver [Estado](#estado).

## Topics actuales

| Topic | Tipo | Publica | Suscribe |
|---|---|---|---|
| `/camera/image_raw` | `sensor_msgs/CompressedImage` | `camera_publisher` | `qr_detector` |
| `/qr_test/image_processed` | `sensor_msgs/CompressedImage` | `qr_detector` | — (para visualizar, ej. `foxglove_bridge`) |

Un solo nodo escribe cada topic — consistente con la regla de "un solo
escritor" de [`docs/CONVENTIONS.md`](../docs/CONVENTIONS.md).

## Cómo ejecutarlo

### Con devcontainer (recomendado)

`.devcontainer/robot_ws` trae ROS2 **Jazzy**, `cv_bridge`, `libzbar0` (para
`pyzbar`) y compila el workspace automáticamente al crear el contenedor
(`postCreateCommand`). En VS Code: *Reopen in Container*, y luego dentro del
contenedor:

```bash
cd /workspace/robot_ws
source install/setup.bash
ros2 run robot_vision camera_publisher   # terminal 1
ros2 run robot_vision qr_detector        # terminal 2
```

### Manual (sin devcontainer)

```bash
sudo apt install ros-jazzy-cv-bridge ros-jazzy-image-transport libzbar0
pip install pyzbar

cd robot_ws
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
source install/setup.bash

ros2 run robot_vision camera_publisher
ros2 run robot_vision qr_detector
```

No hay `launch file` todavía — cada nodo se corre por separado en su propia
terminal (ver [Estado](#estado)).

## Estado

- [x] `robot_vision` — cámara USB + detección de QR (`pyzbar`)
- [ ] Launch file que levante los nodos de `robot_vision` juntos
- [ ] `robot_bringup` / `robot_core` — navegación, LiDAR + SLAM, agregación
      de datos. Existe una exploración previa sin integrar en
      [`robot_ws_legacy/`](../robot_ws_legacy)
- [ ] Puente Serial con `firmware/motors` y `firmware/sensors`
- [ ] `rosbridge_suite` / `web_video_server` hacia `dashboard`

## Ver también

- [`docs/ARCHITECTURE.md`](../docs/ARCHITECTURE.md) — arquitectura general de los 4 componentes
- [`docs/CONVENTIONS.md`](../docs/CONVENTIONS.md) — convenciones transversales
- [`docs/protocols/telemetry-protocol.md`](../docs/protocols/telemetry-protocol.md) — protocolo `robot_ws` ↔ `dashboard` (borrador)
