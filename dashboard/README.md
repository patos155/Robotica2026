# Dashboard

Interfaz Vue 3 para visualizar `/camera/image_raw` y ejecutar la acción
ROS 2 `/run_test` con pruebas bajo demanda (por ahora, `qr`).

## Enlaces utilizados

- WebSocket rosbridge: `ws://IP_DEL_ROBOT:9090`
- Video MJPEG: `http://IP_DEL_ROBOT:8080/stream?topic=/camera/image_raw&type=ros_compressed&qos_profile=sensor_data`
  (`topic` es el topic base; `type=ros_compressed` hace que `web_video_server`
  se suscriba a `/camera/image_raw/compressed`, publicado por
  `image_compressor`, y reenvíe esos JPEG tal cual, sin recomprimir ni
  necesitar el plugin `compressed_image_transport`. `qos_profile=sensor_data`
  es obligatorio: el default de `web_video_server` es `RELIABLE` y el topic
  se publica `BEST_EFFORT`, así que sin esto nunca llegan frames. La
  calidad/resolución las define `image_compressor` y la cámara, no la URL)
- Acción: `/run_test` (`robot_interfaces/action/RunTest`)

## Ejecutar ROS 2

En la laptop del robot, después de compilar y cargar el workspace:

```bash
cd robot_ws
source /opt/ros/humble/setup.bash
colcon build --symlink-install
source install/setup.bash

ros2 launch robot_vision robot_vision.launch.py
```

Esto levanta los tres nodos de `robot_vision`, `rosbridge_server` (puerto
9090) y `web_video_server` (puerto 8080) juntos. Si `rosbridge_server`/
`web_video_server` no están instalados en el devcontainer:

```bash
sudo apt install ros-humble-rosbridge-suite ros-humble-web-video-server
```

## Ejecutar la interfaz

```bash
cd dashboard
npm install
npm run dev
```

Abre `http://localhost:5173`. En **Conexión ROS**, escribe la IP de la laptop
del robot. La interfaz recuerda esa IP, intenta reconectarse automáticamente y
mantiene el video fuera del canal WebSocket para no bloquear la telemetría.

## Configuración opcional

Copia `.env.example` a `.env.local` si quieres cambiar los valores iniciales.
La IP también puede cambiarse directamente desde la interfaz.

## Build de competencia

```bash
npm run build
```

El resultado queda en `dist/` y puede servirse con cualquier servidor HTTP.
