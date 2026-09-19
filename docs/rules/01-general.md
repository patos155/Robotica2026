# Reglas generales del robot y la operación

← [Índice](./README.md)

## Contexto general

RoboCup Rescue Robot League evalúa robots que asisten a servicios de
emergencia en entornos peligrosos — estructuras parcialmente
colapsadas, terrenos irregulares — desde una distancia segura para el
operador humano. La competencia mide tres capacidades:

| Categoría | Qué mide |
|---|---|
| **Mobility (MOB)** | Moverse de forma confiable por distintos terrenos, superar obstáculos, mantener estabilidad, posicionarse donde se necesite realizar una tarea |
| **Dexterity (DEX)** | Manipular objetos con precisión — con cualquier parte del robot, no solo un brazo dedicado |
| **Autonomy (AUTO)** | Reducir la carga cognitiva del operador mediante autonomía confiable — mapeo 2D/3D, identificación de objetos, navegación y manipulación sin intervención humana |

## Requisitos físicos del robot

| Requisito | Valor |
|---|---|
| Peso máximo | 80 kg |
| Abertura para calificar como "robot pequeño" | 30 cm cuadrados |
| Manijas de transporte | Obligatorias, seguras |
| Parada de emergencia (e-stop) | Obligatoria |

## Estación del operador remoto

- El operador **no debe tener contacto visual directo** con el robot
  dentro del carril de prueba — toda la conciencia situacional llega
  por la interfaz del sistema, como si el robot estuviera dentro de
  una estructura real.
- Un solo operador a la vez (cambiar de operador está permitido, operar
  en simultáneo no).
- No se permite comunicación entre el operador y el resto del equipo
  durante la prueba, salvo en un reinicio.
- El operador debe permanecer en su estación durante toda la prueba.

Esta regla es la que justifica directamente el diseño completo del
`dashboard` y la separación entre `robot_ws` y la estación del
operador que ya se documentó en `docs/ARCHITECTURE.md`.

## Tethers (cables)

- Permitidos explícitamente, como medio de comunicación segura y/o
  alimentación continua.
- Deben poder enrollarse en el robot y actuar como malacate para
  ayudar a subir/bajar escaleras si es necesario.
- Si el cable se arrastra dentro del carril, debe manejarse desde la
  entrada por un único **manejador de cable designado** — no se
  permiten palos ni herramientas de apoyo, y el cable no puede pasar
  por encima de las paredes del carril.

## Estructura temporal de una prueba (trial)

Las pruebas comienzan cada 30 minutos:

| Fase | Duración |
|---|---|
| Preparación | 5 min |
| Operación (mobility + dexterity) | 20 min |
| Evaluación de sensores (victim box) | 2 min |
| Salida | 3 min |

No es una carrera — el tiempo alcanza para repeticiones suficientes
que demuestren confiabilidad estadística, no solo velocidad.

## Reinicios (resets) y cambios de configuración

- Un reinicio dura al menos 2 minutos; el robot se puede cargar de
  vuelta a la zona final anterior para continuar.
- Reparaciones deben hacerse dentro del área de arena, no en el
  paddock del equipo.
- **Tocar el robot durante la prueba cuenta como reinicio automático.**
- Cambios de configuración (ej. quitar el brazo) **no** están
  permitidos durante una prueba, pero sí entre pruebas — la
  locomoción debe mantenerse igual.

## Requisitos de acceso al área

Zapato cerrado y pantalón largo obligatorios para entrar al área de
arena — regla operativa del evento, no de diseño del robot.

---
→ Siguiente: [Movilidad](./02-movilidad.md)
