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
│   ├── robot_interfaces/           ← paquete ament_cmake (rosidl)
│   │   ├── package.xml
│   │   ├── CMakeLists.txt
│   │   └── action/
│   │       └── RunTest.action      ← interfaz generica para correr una prueba bajo demanda
│   └── robot_vision/                ← paquete ament_python
│       ├── package.xml
│       ├── setup.py / setup.cfg
│       ├── resource/
│       ├── launch/
│       │   └── robot_vision.launch.py ← levanta los 3 nodos + rosbridge_server + web_video_server juntos
│       ├── robot_vision/
│       │   ├── camera_raw_publisher.py ← nodo: abre la cámara USB, publica /camera/image_raw sin comprimir
│       │   ├── image_compressor.py     ← nodo: suscribe a /camera/image_raw, publica /camera/image_raw/compressed (JPEG)
│       │   ├── qos.py                  ← QoS compartido por los topics de frames de camara
│       │   ├── test_runner.py          ← nodo: Action server /run_test, despacha por test_id
│       │   └── detectors/
│       │       └── qr_detector.py      ← funcion pura detect_qr(frame), usada por test_runner
│       └── test/                        ← linters ament (copyright, flake8, pep257) + test_qr_detector.py
├── build/ install/ log/            ← generados por colcon, ignorados por git
```

`robot_bringup` y `robot_core` (navegación, agregación de datos) descritos en
el README raíz todavía no existen como paquetes — ver [Estado](#estado).

## Topics y actions actuales

| Topic/Action | Tipo | Publica | Suscribe |
|---|---|---|---|
| `/camera/image_raw` | `sensor_msgs/Image` | `camera_raw_publisher` | `image_compressor`, `test_runner` |
| `/camera/image_raw/compressed` | `sensor_msgs/CompressedImage` | `image_compressor` | — (para visualizar, ej. `foxglove_bridge`, y eventualmente el puente hacia `dashboard`) |
| `/run_test` | Action `robot_interfaces/RunTest` | — | `test_runner` (server) |

El tópico crudo (`/camera/image_raw`) nunca sale de esta máquina — solo el
comprimido está pensado para cruzar la red hacia `dashboard`. Un solo nodo
escribe cada topic — consistente con la regla de "un solo escritor" de
[`docs/CONVENTIONS.md`](../docs/CONVENTIONS.md).

## Cómo ejecutarlo

### Con devcontainer (recomendado)

`.devcontainer/robot_ws` trae ROS2 **Humble** (misma versión que la laptop
real del robot), `cv_bridge`, `libzbar0` (para `pyzbar`), `rosbridge_suite`
y `web_video_server`, y compila el workspace automáticamente al crear el
contenedor (`postCreateCommand`). En VS Code: *Reopen in Container*, y
luego dentro del contenedor:

```bash
cd /workspace/robot_ws
source install/setup.bash
ros2 launch robot_vision robot_vision.launch.py
```

Esto levanta los 3 nodos de `robot_vision` más `rosbridge_server` (9090) y
`web_video_server` (8080) para el puente hacia `dashboard`. Con eso
corriendo, pide una prueba desde otra terminal — ver
[Ejecutar una prueba de detección](#ejecutar-una-prueba-de-detección).

Para debug puntual, cada nodo también se puede correr suelto en su propia
terminal:

```bash
ros2 run robot_vision camera_raw_publisher
ros2 run robot_vision image_compressor
ros2 run robot_vision test_runner
```

### Manual (sin devcontainer)

```bash
sudo apt install ros-humble-cv-bridge ros-humble-image-transport libzbar0 \
  ros-humble-rosbridge-suite ros-humble-web-video-server ros-humble-launch-xml
pip install pyzbar "qrcode[pil]"

cd robot_ws
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
source install/setup.bash

ros2 launch robot_vision robot_vision.launch.py
```

### Ejecutar una prueba de detección

Las pruebas (QR, y las que se agreguen después) se piden bajo demanda al
Action `/run_test` — no corren solas ni con cada frame. Con
`camera_raw_publisher` y `test_runner` corriendo:

```bash
ros2 action send_goal /run_test robot_interfaces/action/RunTest \
  "{test_id: 'qr', timeout_s: 0}" --feedback
```

`--feedback` imprime cada `RunTest.Feedback` que llegue mientras la prueba
corre — las instantáneas (QR, hazmat) no mandan ninguno; las de duración
(movimiento, voz) sí. El resultado final trae `success`, `message`,
`result_text` y `result_images`. `timeout_s: 0` usa el default de la prueba
(30s para las de duración); un valor > 0 lo sobreescribe.

Los `test_id` disponibles son las llaves de `TEST_HANDLERS` en
`test_runner.py` — pedir uno que no existe regresa `ABORTED` con un mensaje
de error, no una excepción.

## Estado

- [x] `robot_vision` — cámara cruda + compresión separadas, prueba de QR (`pyzbar`) bajo demanda vía Action `/run_test`
- [x] Launch file que levanta los nodos de `robot_vision` juntos
- [x] `rosbridge_suite` / `web_video_server` hacia `dashboard`
- [ ] Handlers de `test_runner` para hazmat, movimiento y voz (la interfaz `RunTest` ya los soporta)
- [ ] `robot_bringup` / `robot_core` — navegación, LiDAR + SLAM, agregación
      de datos. Existe una exploración previa sin integrar en
      [`robot_ws_legacy/`](../robot_ws_legacy)
- [ ] Puente Serial con `firmware/motors` y `firmware/sensors`

## Ver también

- [`CONVENTIONS.md`](./CONVENTIONS.md) — cómo crear un paquete/interfaz nuevo y cómo agregar una prueba de detección nueva
- [`docs/ARCHITECTURE.md`](../docs/ARCHITECTURE.md) — arquitectura general de los 4 componentes
- [`docs/CONVENTIONS.md`](../docs/CONVENTIONS.md) — convenciones transversales
- [`docs/protocols/telemetry-protocol.md`](../docs/protocols/telemetry-protocol.md) — protocolo `robot_ws` ↔ `dashboard` (borrador)
