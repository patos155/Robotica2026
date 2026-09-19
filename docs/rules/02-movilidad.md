# Movilidad

← [Índice](./README.md) | ← [General](./01-general.md)

## Arena y estructura de las pistas

### Layout general de la arena

La arena se organiza en varias "secuencias" temáticas: aproximación
urbana, aproximación semi-colapsada, acceso a vivienda, y búsqueda en
vivienda (laberinto). Cada una agrupa terrenos y obstáculos
específicos — ver más abajo el detalle de cada uno.

<div align="center">
  <figure>
    <img src="./assets/02-movilidad/arena-layout-2026.jpg" width="640" alt="Plano de la arena de 125 por 115 pies con bloques de mesas, zona RMRC en cian y los carriles a la derecha y abajo">
    <figcaption>
      <b>Figura 2.1</b> — Arena Layout 2026: 125 × 115 ft (38 × 35 m). Los carriles de las secuencias quedan en el borde derecho e inferior, la zona RMRC en cian abajo a la izquierda y una zona amarilla de 44 × 16 ft arriba a la derecha.<br>
      <i>Fuente: Rules 2026D, p. 2</i>
    </figcaption>
  </figure>
</div>

🚩 **PENDIENTE:** las dimensiones exactas de la arena completa (125 x
115 pies según el PDF de referencia) probablemente no aplican tal cual
para la arena de TMR, que seguramente es más pequeña. Confirmar
dimensiones reales del venue de TMR.

### Estructura de un carril individual

Un carril individual mide aproximadamente 2.4m x 4.8m, con zonas de
inicio/fin en ambos extremos, una sección central ("crossover"), y
puntos de tareas lineales/omni embebidos en el trayecto.

<div align="center">
  <figure>
    <img src="./assets/02-movilidad/img69.png" width="340" alt="Plano de un carril de 2.4 por 4.8 metros con zonas de inicio, negotiate, lineal, omni y crossover">
    <figcaption>
      <b>Figura 2.2</b> — Lane Overview: carril de 2.4 × 4.8 m con zonas START/END de 1.2 m en cada extremo, obstáculos NEGOTIATE 1 y 2, aparatos LINEAR y OMNI, y el CROSSOVER al centro.<br>
      <i>Fuente: Rules 2026D, p. 6 (captura de pantalla del PDF)</i>
    </figcaption>
  </figure>
</div>

### Secuencias de misión

**Pruebas de carril único (single lane)** — preliminares, cada carril
se prueba de forma individual para capturar repeticiones estadísticas.

**Secuencias multi-carril (semis/finales)** — 4 secuencias concurrentes
con objetivos distintos, el orden es libre pero sin repetir carriles
hasta completar los 4:

| Secuencia | Carriles |
|---|---|
| 1 | Crossing Ramps, Gravel, Traverse & Center |
| 2 | Cubic Stepfield, K-Rails, Pallet Hurdles |
| 3 | Avoid Holes, Stairs, Doors |
| 4 | Labyrinth (mapeo) |

🚩 **PENDIENTE:** confirmar si TMR usa esta misma estructura de 4
secuencias o una reducida.

## Terrenos y obstáculos

> **Sobre las fotos de referencia:** el PDF 2026D no trae fotos de la
> mayoría de estos terrenos, así que se tomaron de [NIST/ASTM E54.09 —
> Standard Test Methods for Response Robots, Ground Tests](https://www.nist.gov/document/ground-tests-3-maneuvering-and-mobility-2022a2),
> los métodos de prueba en que se basan. Muestran terrenos equivalentes,
> no necesariamente la arena de 2026: las medidas del reglamento van en
> el texto y en el pie de cada figura.

### Crossing Ramps (con discos deslizantes)

- Pendiente/inclinación: 0° o 15°.
- Incluye "slip disks" (discos deslizantes) — mencionados por nombre
  pero sin especificación de tamaño/material en el PDF.

🚩 **PENDIENTE:** especificación exacta de los "slip disks" (dimensión,
material, coeficiente de fricción esperado) — no está detallada en
el PDF, buscar referencia visual o preguntar a la organización.

<div align="center">
<table>
<tr>
<td align="center" valign="top"><img src="./assets/02-movilidad/nist-crossing-ramps-lane.jpg" width="400" alt="Vista de un carril con rampas cruzadas de madera y dos postes de referencia"></td>
<td align="center" valign="top"><img src="./assets/02-movilidad/nist-crossing-ramps-robot.jpg" width="230" alt="Robot de orugas con una canasta cruzando una rampa"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.3</b> — Crossing Ramps (referencia NIST). Izquierda: vista del carril. Derecha: un robot cruzando una rampa. En 2026D las pendientes son de 15°|30° con "spinners" opcionales (Arena Layout, p. 2).<br>
  <i>Fuente: NIST/ASTM E54.09, Ground Tests, diap. 35</i>
</td>
</tr>
</table>
</div>

### K-Rails

- Altura: 10 cm (según 2026D) — el PDF también menciona variantes de
  10/20cm dependiendo de la sección de arena.
- Pendiente: 0° o 15°.
- Variantes: "avoid omni", con inclinación, con obstáculos tipo
  "negotiate", con "pinch points".

<div align="center">
<table>
<tr>
<td align="center"><img src="./assets/02-movilidad/img91.jpg" width="400" alt="Planta del carril con un objeto omni rojo en el piso al centro"></td>
<td align="center"><img src="./assets/02-movilidad/img90.jpg" width="400" alt="Vista isométrica del carril con un objeto omni rojo en el piso al centro"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.4</b> — K-Rails, 10 cm, variante "avoid omni": un objeto omni (cuadro rojo) queda en el piso al centro. Izquierda: planta. Derecha: isométrica.<br>
  <i>Fuente: Rules 2026D, p. 7</i>
</td>
</tr>
</table>
</div>

<div align="center">
<table>
<tr>
<td align="center"><img src="./assets/02-movilidad/img93.jpg" width="400" alt="Planta del carril con módulo central inclinado 15 grados"></td>
<td align="center"><img src="./assets/02-movilidad/img92.jpg" width="400" alt="Vista isométrica del carril con módulo central inclinado 15 grados"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.5</b> — K-Rails, 10 cm, con pendiente de 15°. Izquierda: planta. Derecha: isométrica.<br>
  <i>Fuente: Rules 2026D, p. 7</i>
</td>
</tr>
</table>
</div>

<div align="center">
<table>
<tr>
<td align="center"><img src="./assets/02-movilidad/img94.jpg" width="400" alt="Planta del carril inclinado con dos barras rojas de negotiate"></td>
<td align="center"><img src="./assets/02-movilidad/img95.jpg" width="400" alt="Vista isométrica del carril inclinado con dos barras rojas de negotiate"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.6</b> — K-Rails, 10 cm, pendiente de 15° y obstáculos Negotiate (barras rojas en las zonas de inicio/fin). Izquierda: planta. Derecha: isométrica.<br>
  <i>Fuente: Rules 2026D, p. 7</i>
</td>
</tr>
</table>
</div>

<div align="center">
<table>
<tr>
<td align="center"><img src="./assets/02-movilidad/img97.jpg" width="400" alt="Planta del carril inclinado con paneles rojos en V"></td>
<td align="center"><img src="./assets/02-movilidad/img96.jpg" width="400" alt="Vista isométrica del carril inclinado con paneles rojos en V"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.7</b> — K-Rails, 10 cm, pendiente de 15° y "pinch points" (paneles rojos en V que estrechan el paso). Izquierda: planta. Derecha: isométrica.<br>
  <i>Fuente: Rules 2026D, p. 7</i>
</td>
</tr>
</table>
</div>

### Gravel (grava)

- Pendiente: 15°.
- Terreno "inestable" con marcos en X embebidos (según descripción de
  "semi-collapsed approach sequence" del reglamento de TMR/2026D).

🚩 **PENDIENTE:** tipo/tamaño de grava exacto — relevante para que
mecánica calibre tracción y garra de las llantas/orugas.

<div align="center">
<table>
<tr>
<td align="center" valign="top"><img src="./assets/02-movilidad/nist-gravel-1.jpg" width="240" alt="Robot de orugas con brazo avanzando sobre grava suelta"></td>
<td align="center" valign="top"><img src="./assets/02-movilidad/nist-gravel-2.jpg" width="240" alt="Robot con flippers verdes sobre grava junto a la pared del carril"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.8</b> — Terreno de grava (referencia NIST): dos robots de orugas, uno con flippers, sobre grava suelta. En 2026D es un terreno inestable con marcos en X embebidos (Arena Layout, p. 2).<br>
  <i>Fuente: NIST/ASTM E54.09, Ground Tests, diap. 37</i>
</td>
</tr>
</table>
</div>

### Cubic Stepfield (campo de escalones cúbicos)

- Pendiente: 0° o 15°.
- Terreno de bloques de altura variable que fuerza el chasis a
  adaptarse — común en RoboCupRescue, sin dimensión exacta de los
  cubos especificada en el extracto disponible.

🚩 **PENDIENTE:** dimensiones exactas de los cubos individuales.

<div align="center">
  <figure>
    <img src="./assets/02-movilidad/nist-stepfield.jpg" width="640" alt="Tres stepfields de cubos de madera con un carro todoterreno, un robot cuadrúpedo y un robot de orugas con patas">
    <figcaption>
      <b>Figura 2.9</b> — Stepfields de cubos de madera en tres instalaciones (NIST, RACE y RoboCupRescue Australia). En 2026D se usan "Half-Cubic Stepfields" con escalones de 15 cm (Arena Layout, p. 2).<br>
      <i>Fuente: NIST/ASTM E54.09, Ground Tests, diap. 42</i>
    </figcaption>
  </figure>
</div>

### Traverse & Center

- Inclinación: 15°, ancho variable.
- Tarea de centrarse dentro de un pasillo de ancho variable mientras
  se avanza en pendiente.

<div align="center">
<table>
<tr>
<td align="center"><img src="./assets/02-movilidad/nist-centering-1.jpg" width="400" alt="Robot de orugas entre dos paredes de madera, con el espacio igual a su diagonal"></td>
<td align="center"><img src="./assets/02-movilidad/nist-centering-2.jpg" width="400" alt="Robot de orugas entre dos paredes de madera, con el espacio al 120 por ciento de su ancho"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.10</b> — Centering (referencia NIST). Izquierda: espacio igual a la diagonal del robot. Derecha: 120% de su ancho, para favorecer la autonomía. En 2026D los pasos se ajustan al ancho del robot + 10 cm (Arena Layout, p. 2).<br>
  <i>Fuente: NIST/ASTM E54.09, Ground Tests, diap. 15</i>
</td>
</tr>
</table>
</div>

### Pallet Hurdles with Pipes

- Elevación: 20 cm.

<div align="center">
  <figure>
    <img src="./assets/02-movilidad/nist-hurdle.jpg" width="420" alt="Robot de orugas con brazo superando un obstáculo de madera con un tubo blanco en el borde">
    <figcaption>
      <b>Figura 2.11</b> — Obstáculo tipo hurdle con tubo en el borde (referencia NIST): un robot con brazo lo supera. En 2026D son "Pallets &amp; Pipes" con escalones de 20 cm (Arena Layout, p. 2).<br>
      <i>Fuente: NIST/ASTM E54.09, Ground Tests, diap. 23</i>
    </figcaption>
  </figure>
</div>

### Doors (puertas)

- Tipo perilla de palanca, 90 cm de ancho, con cierre con peso.
- Altura de la perilla: 90-120 cm.

🖼️ **PENDIENTE (imagen):** foto de las puertas. No la incluyen ni el PDF
2026D ni el material de NIST revisado (solo una miniatura ilegible de
"Open Doors").

### Stairs (escaleras)

- Inclinación: 35° / 40° / 45°.
- Variantes con 0, 2 o 4 barreras.
- "Stair debris": escalones de 20cm de altura.

<div align="center">
<table>
<tr>
<td align="center" valign="top"><img src="./assets/02-movilidad/nist-stairs.jpg" width="420" alt="Robot de orugas con brazo subiendo una escalera de madera dentro de un marco"></td>
<td align="center" valign="top"><img src="./assets/02-movilidad/nist-stairs-debris.jpg" width="230" alt="Robot de orugas subiendo escalones con tablones sueltos"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.12</b> — Escaleras (referencia NIST). Izquierda: un robot con brazo sube la escalera. Derecha: variante con "debris" (tablones sueltos sobre los escalones). En 2026D el debris es de 20 cm (Arena Layout, p. 2).<br>
  <i>Fuente: NIST/ASTM E54.09, Ground Tests, diap. 29 y 31</i>
</td>
</tr>
</table>
</div>

### Avoid Holes Lane

- Camino elevado a seguir, penaliza tumbar postes: 10s de penalización
  de tiempo por cada uno derribado.

<div align="center">
<table>
<tr>
<td align="center" valign="top"><img src="./assets/02-movilidad/nist-avoid-holes-photo.jpg" width="420" alt="Robot de orugas junto a una zona del piso marcada con cinta roja"></td>
<td align="center" valign="top"><img src="./assets/02-movilidad/nist-avoid-holes-path.jpg" width="230" alt="Trazado en planta de un camino de palets en zigzag con START y END"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.13</b> — Avoid Holes (referencia NIST). Izquierda: un robot junto a una zona marcada con cinta roja. Derecha: trazado en planta de un camino de palets con inicio/fin; hay que seguirlo sin salirse, con cambios de altura. En 2026D son 10 cm de elevación y 5 puertas de 90 cm de ancho (Arena Layout, p. 2).<br>
  <i>Fuente: NIST/ASTM E54.09, Ground Tests, diap. 18 y 19</i>
</td>
</tr>
</table>
</div>

## Desafíos adicionales (puntos extra)

Retos opcionales que se hacen durante los carriles; los puntos están en
la Mission Form del PDF.

### Carry a payload (pipestar)

Recoger el objeto con el gripper —antes de la misión o durante ella,
ante un juez— y llevarlo en todos los carriles. Puede ser un
*pipestar* u otro objeto anunciado al inicio de la competencia. Peso
máximo y ancho de gripper en
[`06-puntuacion-premios.md`](./06-puntuacion-premios.md).

<div align="center">
<table>
<tr>
<td align="center"><img src="./assets/02-movilidad/img47.jpg" width="460" alt="Robot con brazo y gripper sobre palets en un carril, con el pipestar en el gripper"></td>
<td align="center"><img src="./assets/02-movilidad/img48.jpg" width="233" alt="Primer plano del gripper sujetando el pipestar de madera"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.14</b> — Carry a pipestar. Izquierda: robot con brazo cruzando el carril con la carga. Derecha: detalle del gripper sujetándola.<br>
  <i>Fuente: Rules 2026D, p. 4</i>
</td>
</tr>
</table>
</div>

### Negotiate Leaning Obstacles

Barras inclinadas que hay que cruzar sin tocar. Están magnetizadas, así
que cualquier barra tocada cae (un golpe fuerte a las paredes también
las tira). Las caídas quedan como escombro hasta que el robot pasa a la
siguiente repetición o terreno. Puede haber puntos extra por las barras
que sigan en pie en cada repetición.

<div align="center">
<table>
<tr>
<td align="center"><img src="./assets/02-movilidad/img49.jpg" width="442" alt="Robot cuadrúpedo amarillo cruzando entre barras de madera inclinadas"></td>
<td align="center"><img src="./assets/02-movilidad/img50.jpg" width="177" alt="Detalle de la unión entre dos barras de madera con una arandela metálica"></td>
</tr>
<tr>
<td align="center" colspan="2">
  <b>Figura 2.15</b> — Negotiate Leaning Obstacles. Izquierda: un robot cuadrúpedo cruza entre las barras inclinadas. Derecha: detalle de la unión de una barra (magnetizada, por eso cae al tocarla).<br>
  <i>Fuente: Rules 2026D, p. 4</i>
</td>
</tr>
</table>
</div>

### Radio Comms Degradation

Misión bajo condiciones de red cada vez peores, usando la Radio
Degradation Box oficial. Parámetros y referencia de la caja en
[`07-degradacion-comunicaciones.md`](./07-degradacion-comunicaciones.md).

<div align="center">
  <figure>
    <img src="./assets/02-movilidad/img51.jpg" width="640" alt="Fila de mesas frente a la arena con equipos circulados en rojo">
    <figcaption>
      <b>Figura 2.16</b> — Radio Comms Degradation Setup: los equipos circulados en rojo están sobre las mesas, frente a la arena.<br>
      <i>Fuente: Rules 2026D, p. 4</i>
    </figcaption>
  </figure>
</div>

## Puntuación de Mobility

- Recorrido teleoperado exitoso (ida o vuelta): **1 punto**.
- Recorrido autónomo exitoso, sin intervención (hands-off): **15
  puntos** (2026D) — ver diferencia con 2026A en
  [`08-diferencias-version.md`](./08-diferencias-version.md).
- El operador puede tomar control a mitad de un intento autónomo para
  terminarlo teleoperado (1 punto) y reintentar autónomo la siguiente
  repetición.
- Puntos extra disponibles por: cargar un payload, "Negotiate Leaning
  Obstacles", y degradación de comunicaciones activa (ver
  [`07-degradacion-comunicaciones.md`](./07-degradacion-comunicaciones.md)).
- Puntajes de mobility se normalizan contra el máximo alcanzado por
  cualquier equipo en ese carril.

---
← Anterior: [General](./01-general.md) | → Siguiente: [Destreza](./03-destreza.md)
