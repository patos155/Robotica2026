# Convenciones — robot_ws

Convenciones específicas de este workspace ROS2. Para convenciones de Git y
flujo de trabajo del repositorio, ver `CONTRIBUTING.md` en la raíz. Para
convenciones transversales al proyecto completo, ver
[`docs/CONVENTIONS.md`](../docs/CONVENTIONS.md).

## Idioma

Código en inglés (paquetes, nodos, topics, variables, funciones).
Comentarios y documentación en español — regla transversal del repo.

## Nomenclatura

| Elemento | Convención | Ejemplo |
|---|---|---|
| Paquetes | `snake_case`, prefijo `robot_` | `robot_vision`, `robot_interfaces` |
| Nodos, topics, entry points | `snake_case` | `camera_raw_publisher`, `/camera/image_raw` |
| Clases (nodos, tipos de interfaz) | `PascalCase` | `CameraRawPublisher`, `RunTest` |
| Archivos de interfaz (`.action`/`.srv`/`.msg`) | `PascalCase`, un archivo = un tipo | `RunTest.action` |
| Funciones de detección puras | `detect_<algo>` | `detect_qr` |
| Constantes | `UPPER_SNAKE_CASE`, sin números mágicos sueltos | `JPEG_QUALITY`, `CAMERA_INDEX` |

## Tipos de paquete: ¿`ament_python` o `ament_cmake`?

- **`ament_python`** (ej. `robot_vision`): nodos en Python puro, sin definir
  tipos de mensaje/servicio/action nuevos. Es el default — más simple de
  compilar y de iterar.
- **`ament_cmake` + `rosidl`** (ej. `robot_interfaces`): **obligatorio** si
  el paquete define su propio `.msg`/`.srv`/`.action`. ROS2 no permite
  generar interfaces custom dentro de un paquete `ament_python` puro, sin
  importar en qué lenguaje estén los nodos que las van a usar.

Regla práctica: si solo necesitas nodos, va en un paquete `ament_python`
existente o uno nuevo. Si necesitas un tipo de mensaje nuevo, ese tipo vive
en `robot_interfaces` — no se crea un paquete de interfaces por cada tipo,
es uno solo para todo el workspace hasta que haya una razón real de
separarlo.

## Cómo crear un paquete `ament_python` nuevo

```bash
cd robot_ws/src
ros2 pkg create --build-type ament_python --node-name mi_nodo mi_paquete
```

Genera `package.xml`, `setup.py`, `setup.cfg`, `resource/`, y un nodo de
ejemplo. Después:

1. Declarar las dependencias reales en `package.xml` (`<depend>rclpy</depend>`,
   etc. — ver `robot_vision/package.xml` como referencia).
2. Registrar cada nodo ejecutable en `setup.py` → `entry_points['console_scripts']`.
3. `colcon build --symlink-install --packages-select mi_paquete`.

## Cómo agregar una interfaz nueva (Action/Service/Msg)

Casi siempre esto significa agregar un tipo a `robot_interfaces`, no crear
un paquete de interfaces nuevo:

1. Crear el archivo en `robot_interfaces/action/`, `srv/` o `msg/` según el
   tipo (ver `action/RunTest.action` como referencia del formato: goal /
   `---` / result / `---` / feedback para actions).
2. Agregarlo a `rosidl_generate_interfaces()` en
   `robot_interfaces/CMakeLists.txt`:
   ```cmake
   rosidl_generate_interfaces(${PROJECT_NAME}
     "action/RunTest.action"
     "srv/MiServicio.srv"        # nuevo
     DEPENDENCIES sensor_msgs
   )
   ```
   Si el tipo usa un mensaje de otro paquete (ej. `sensor_msgs/CompressedImage`),
   ese paquete debe estar en `DEPENDENCIES` aquí y en `<depend>` de
   `package.xml`.
3. `colcon build --packages-select robot_interfaces` regenera los bindings
   de Python (`from robot_interfaces.action import MiNuevoTipo`).
4. Antes de crear un tipo nuevo, pregúntate si `RunTest` ya alcanza —
   está diseñado como genérico (`test_id` + payload de resultado)
   precisamente para no tener que agregar una interfaz por cada prueba
   nueva. Ver la siguiente sección.

## Cómo agregar una nueva prueba/detección

Todas las pruebas bajo demanda (QR, y las que sigan: hazmat, movimiento,
voz) comparten la misma interfaz `RunTest` y se despachan desde
`test_runner.py` por `test_id` — **no** se crea un nodo ni una interfaz
nueva por cada prueba. Pasos:

1. Escribir la lógica de detección como **función pura** en
   `robot_vision/robot_vision/detectors/<nombre>_detector.py`: recibe un
   frame (u otra fuente de datos) y regresa el resultado, sin tocar
   `rclpy`/`Node`. Ejemplo: `detect_qr(frame) -> (annotated_frame, text)`
   en `detectors/qr_detector.py`.
2. En `test_runner.py`, escribir un handler
   `_handle_<nombre>(node, goal_handle) -> RunTest.Result` que llame a esa
   función, llene `result.success` / `result.message` / `result.result_text`
   / `result.result_images`, y llame `goal_handle.succeed()` o
   `goal_handle.abort()`.
   - Prueba **instantánea** (un solo frame, como QR/hazmat): usa
     `node.latest_frame`, procesa una vez, regresa. No manda feedback. Ver
     `_handle_qr` en `test_runner.py` como referencia.
   - Prueba de **duración variable** (como movimiento/voz): hace su propio
     loop dentro del handler, publica `goal_handle.publish_feedback(...)`
     periódicamente, revisa `goal_handle.is_cancel_requested` en cada
     vuelta, y aplica un timeout (30s si el cliente no manda uno distinto
     en `timeout_s`). Ver `_handle_motion` en `test_runner.py` como
     referencia — incluye el patrón de correr algo con estado propio
     (`cv2.BackgroundSubtractorMOG2`) que se crea nuevo en cada prueba.
3. Registrar el handler en el dict `TEST_HANDLERS` de `test_runner.py`:
   ```python
   TEST_HANDLERS = {
       'qr': _handle_qr,
       'hazmat': _handle_hazmat,   # nuevo
   }
   ```
4. Si la prueba necesita una fuente de datos distinta a la cámara (ej.
   audio para voz), agregar la suscripción/captura correspondiente en
   `TestRunner.__init__`, siguiendo el mismo patrón que `self.latest_frame`
   (un solo escritor: el callback correspondiente).
5. Agregar `robot_vision/test/test_<nombre>_detector.py` que llame la
   función pura con datos sintéticos (ver `test_qr_detector.py` como
   referencia) — no hace falta levantar ROS2 para probar la lógica de
   detección, solo `colcon test --packages-select robot_vision`.

No se agrega una interfaz nueva, un action server nuevo, ni un nodo nuevo
para esto — ese es justo el problema que `RunTest` genérico + el dict de
handlers resuelve.

## Reglas de arquitectura (no negociables)

- Un solo nodo escribe cada topic (regla transversal, ver
  `docs/CONVENTIONS.md`). El tópico crudo `/camera/image_raw` lo escribe
  solo `camera_raw_publisher`.
- El QoS de los topics de cámara vive **solo** en `qos.py` (`CAMERA_QOS`) —
  no se define un `QoSProfile` inline en un nodo nuevo. Un mismatch de QoS
  (ej. usar el atajo entero `10` en vez de `CAMERA_QOS`) no da error, el
  subscriber simplemente nunca recibe nada.
- `/camera/image_raw` (crudo, sin comprimir) nunca sale de esta máquina —
  solo `/camera/image_raw/compressed` está pensado para cruzar la red
  hacia `dashboard`.
- Los detectores (`detectors/*.py`) son funciones puras sin estado de
  `Node` — el estado de ROS2 (suscripciones, `latest_frame`, el action
  server) vive únicamente en `test_runner.py`.
- `test_runner.py` es el único despachador de `test_id` — un handler nuevo
  se agrega al dict `TEST_HANDLERS`, no como un nodo/action server
  independiente.

## Logging

Logger nativo de ROS2 (`self.get_logger()`), nunca `print()` suelto —
regla transversal, ver [`docs/CONVENTIONS.md`](../docs/CONVENTIONS.md).
