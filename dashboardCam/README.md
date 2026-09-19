# DashboardCam

Interfaz Vue 3 para visualizar `/inspection/image_processed` y ejecutar la
acción ROS 2 `/execute_test` con la prueba `qr`.

## Enlaces utilizados

- WebSocket rosbridge: `ws://IP_DEL_ROBOT:9090`
- Video MJPEG: `http://IP_DEL_ROBOT:8080/stream?topic=/inspection/image_processed`
- Acción: `/execute_test` (`robot_interfaces/action/ExecuteTest`)

## Ejecutar ROS 2

En la laptop del robot, abre terminales separadas después de compilar y cargar
el workspace:

```bash
cd robot_ws2
source /opt/ros/jazzy/setup.bash
colcon build --symlink-install
source install/setup.bash
```

```bash
ros2 run robot_vision camera_stream
ros2 run robot_vision inspection_pipeline
ros2 launch rosbridge_server rosbridge_websocket_launch.xml port:=9090
ros2 run web_video_server web_video_server --ros-args -p port:=8080
```

Los cuatro procesos deben permanecer activos. Si no están instalados los
servidores web:

```bash
sudo apt install ros-jazzy-rosbridge-suite ros-jazzy-web-video-server
```

## Ejecutar la interfaz

```bash
cd dashboardCam
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
