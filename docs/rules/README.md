# Reglas de referencia — RoboCup Rescue Robot League

Documentación del reglamento oficial, organizada por tema para que
cada parte del equipo consulte solo lo que le aplica.

**Fuente:** RoboCup Rescue Robot League, 2026 Championship (Icheon,
Corea del Sur). TMR publicó en
[femexrobotica.org/tmr2026](https://femexrobotica.org/tmr2026/index.php/categorias/robocup-major/rescue-robot/)
el reglamento **2026A_Draft**, mientras que la versión internacional
más reciente es **2026D**. Los PDF originales están en
[`base/`](./base/).

**Versión de referencia principal:** 2026D (más reciente, con changelog
del 6/3/2026) — decisión del equipo. Cuando se publique el reglamento
del siguiente año se corregirán estos documentos. Las diferencias con
2026A están en [`08-diferencias-version.md`](./08-diferencias-version.md).

## Leyenda de flags

Estos documentos marcan explícitamente dónde falta información o
trabajo, para poder escanear rápido sin releer todo:

| Flag | Significa |
|---|---|
| 🚩 **PENDIENTE** | El reglamento no da suficiente detalle — falta investigar, preguntar a la organización, o decidir como equipo |
| 🖼️ **PENDIENTE (imagen)** | Espacio marcado para agregar una foto/diagrama de referencia, aún sin incluir |

Todos los pendientes de todos los archivos están además consolidados
en un solo checklist: [`PENDIENTES.md`](./PENDIENTES.md).

## Figuras

Las imágenes están en [`assets/`](./assets/), una carpeta por archivo.
Cada figura lleva número (Figura 2.3 = tercera figura de
`02-movilidad.md`), una descripción y su fuente:

- **Rules 2026D, p. N** — extraída del PDF oficial. Las marcadas como
  "captura de pantalla" se capturaron a mano, porque el PDF las compone
  a partir de varias capas y no existen como una sola imagen.
- **Referencia NIST** — foto de los métodos de prueba de
  [NIST/ASTM E54.09](https://www.nist.gov/document/ground-tests-3-maneuvering-and-mobility-2022a2),
  usada donde el PDF no trae foto del terreno. Muestra un terreno
  equivalente, no necesariamente la arena de 2026.

## Archivos

| Archivo | Contenido | A quién le interesa principalmente |
|---|---|---|
| [`01-general.md`](./01-general.md) | Contexto, requisitos del robot, operador remoto, tethers, estructura de una prueba, reinicios | Todo el equipo |
| [`02-movilidad.md`](./02-movilidad.md) | Layout de la arena, carriles, cada terreno/obstáculo (con fotos), desafíos adicionales, puntuación de Mobility | Mecánica, `firmware/motors` |
| [`03-destreza.md`](./03-destreza.md) | Tareas de clasificación y operacionales, multiplicadores de autonomía | Mecánica (manipulador), `robot_ws` |
| [`04-caja-victima.md`](./04-caja-victima.md) | Las 6-7 tareas de sensado autónomo (térmica, hazmat, movimiento, imán, QR, audio) | `firmware/sensors`, `robot_vision` |
| [`05-mapeo.md`](./05-mapeo.md) | Formatos de entrega GeoTIFF/PLY/CSV, puntuación de mapeo | `robot_ws` (`robot_core/data_logger.py`) |
| [`06-puntuacion-premios.md`](./06-puntuacion-premios.md) | Normalización general, premios, preguntas frecuentes relevantes | Todo el equipo |
| [`07-degradacion-comunicaciones.md`](./07-degradacion-comunicaciones.md) | Caja de degradación de señal, parámetros de prueba | `robot_ws`, `dashboard` |
| [`08-diferencias-version.md`](./08-diferencias-version.md) | Comparación 2026A (borrador) vs 2026D (revisión) | Referencia histórica |
| [`PENDIENTES.md`](./PENDIENTES.md) | Checklist consolidado de todo lo marcado 🚩 y 🖼️ en los demás archivos | Todo el equipo |
| [`base/`](./base/) | PDF originales: `RoboCupRescue-Rules-2026D.pdf` y `RoboCupRescue-Rules-2026A_Draft.pdf` | Consulta de la fuente |
| [`assets/`](./assets/) | Imágenes de las figuras, por archivo | — |
