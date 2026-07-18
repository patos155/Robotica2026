# Protocolo — `robot_ws` ↔ `dashboard`

**Estado:** borrador — depende de decisiones de `robot_ws` aún no tomadas (ver sección final). El `dashboard` tampoco está iniciado. **Medio:** WiFi, dos canales separados (ver `ARCHITECTURE.md`):

- **Telemetría y comandos** — vía `rosbridge_suite`, expone topics de ROS2 como WebSocket con mensajes JSON. Del lado del navegador, la librería `roslibjs` maneja la conexión y el formato de bajo nivel de rosbridge — este documento describe los **topics y su contenido**, no el envoltorio de rosbridge en sí.
- **Video** — vía `web_video_server`, stream HTTP/MJPEG independiente, consumido directo por un `<img>`/`<video>` en el `dashboard`. No pasa por rosbridge ni por este protocolo de topics.

## Topics propuestos: `robot_ws` → `dashboard`

| Topic | Tipo de mensaje | Frecuencia | Contenido |
|---|---|---|---|
| `/robot/pose` | `geometry_msgs/Pose2D` o `nav_msgs/Odometry` (a decidir) | ~2-5 Hz | Posición y orientación actual del robot |
| `/map` | `nav_msgs/OccupancyGrid` | Cuando cambia | Mapa 2D generado por SLAM — tipo estándar de ROS2, compatible directo con herramientas de visualización |
| `/robot/mode` | `std_msgs/String` (`"MANUAL"` / `"AUTO"`) | Al cambiar | Refleja el switch físico del control remoto |
| `/robot/sensors` | Por decidir — ver más abajo | ~1-2 Hz | Última lectura de cada sensor ambiental |
| `/robot/heartbeat` | `std_msgs/Empty` o timestamp | ~1 Hz | Le permite al `dashboard` medir cuánto tiempo lleva sin señal, para el `ConnectionBanner` |

## Topics/servicios propuestos: `dashboard` → `robot_ws`

| Nombre | Tipo | Descripción |
|---|---|---|
| `/robot/emergency_stop` | Servicio `std_srvs/Trigger` (propuesto) | El operador fuerza una parada. Debe procesarse en `safety_watchdog` o `motors_bridge`, nunca en el nodo de navegación directamente |

## Decisión abierta: ¿tipo de mensaje custom o JSON dentro de `std_msgs/String`?

Para `/robot/sensors` (y potencialmente `/robot/pose`) hay dos caminos:

- **Mensaje custom de ROS2** (`.msg` propio, ej. `SensorReadings.msg` con un campo por sensor) — más "correcto" en términos de ROS2, pero requiere un paquete de interfaces separado (`rosidl_generate_interfaces`) y recompilar cuando cambie un campo.
- **`std_msgs/String` con un JSON adentro** — más simple de implementar y de depurar dado el nivel de experiencia del equipo con ROS2 hasta ahora, a costa de perder el tipado fuerte que ROS2 ofrece.

**Recomendación:** empezar con `std_msgs/String` + JSON, consistente con el criterio ya aplicado en el resto del proyecto de evitar complejidad que no reduce trabajo real dado el tiempo y experiencia del equipo. Confirmar esta decisión antes de implementar `robot_core`.

## Pendiente antes de que este documento pase de "borrador" a "definido"

- [ ] Confirmar tipo de mensaje para `/robot/pose` y `/robot/sensors`
- [ ] Definir el JSON exacto de `/robot/sensors` (probablemente espejo de las lecturas ya definidas en `sensors-protocol.md`)
- [ ] Confirmar si `/robot/emergency_stop` es un servicio o un topic — un servicio da confirmación de que se recibió, un topic es más simple pero sin esa garantía
- [ ] Definir después de cuántos segundos sin `/robot/heartbeat` el `dashboard` debe mostrar el banner de "sin comunicación"