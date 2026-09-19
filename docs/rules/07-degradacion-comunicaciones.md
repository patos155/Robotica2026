# Degradación de comunicaciones

← [Índice](./README.md) | ← [Puntuación y premios](./06-puntuacion-premios.md)

La competencia recompensa explícitamente a los robots que operan de
forma confiable bajo condiciones intermitentes e impredecibles, con
una **caja de degradación de señal oficial** ya publicada como
referencia para entrenar de antemano:
[`github.com/tudo-cni/vsting-sa`](https://github.com/tudo-cni/vsting-sa)
(Communications Network Institute, TU Dortmund).

## Parámetros máximos de referencia para preparación

Pueden ajustarse el día del evento:

| Parámetro | Valor máximo |
|---|---|
| Ancho de banda | 5 Mbps |
| Pérdida de paquetes | 10% |
| Retraso (delay) | 100 ms |

No hay restricción sobre qué tecnología inalámbrica usar, siempre que
se cumpla la normativa local de telecomunicaciones del país sede — esto
da libertad total sobre las decisiones de WiFi propio, radios
dedicados, etc. ya discutidas en `docs/ARCHITECTURE.md`.

Estos parámetros son el objetivo concreto y medible a simular al
probar la robustez del enlace WiFi entre `robot_ws` y `dashboard` —
mejor que "simular mucha interferencia" de forma genérica.

---
← Anterior: [Puntuación y premios](./06-puntuacion-premios.md) | → Siguiente: [Diferencias entre versiones](./08-diferencias-version.md)
