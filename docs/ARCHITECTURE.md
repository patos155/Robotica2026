# Arquitectura del sistema

## Diagrama general

```
┌────────────────────┐        ┌────────────────────┐
│  firmware/motors   │        │ firmware/sensors   │
│  (Arduino)         │        │  (Arduino)         │
│                    │        │                    │
│  Motores           │        │  Gas, temperatura  │
│  Ultrasónicos      │        │  Humedad, sonido   │
│  Control remoto    │        │  Polo de imán, QR* │
└─────────┬──────────┘        └─────────┬──────────┘
          │ Serial (USB)                │ Serial (USB)
          └──────────────┬──────────────┘
                         ▼
                ┌────────────────────┐
                │     robot_ws       │
                │  (laptop robot)    │
                │                    │
                │  ROS2: navegación  │
                │  LiDAR + SLAM      │
                │  agregación datos  │
                └─────────┬──────────┘
                          │ WiFi
             ┌────────────┴────────────┐
             │                         │
    rosbridge_suite            web_video_server
    (telemetría, JSON)          (video, MJPEG/HTTP)
             │                         │
             └────────────┬────────────┘
                          ▼
                ┌───────────────────┐
                │     dashboard     │
                │ (laptop operador) │
                │                   │
                │  React — video,   │
                │  mapa, sensores,  │
                │  control          │
                └───────────────────┘
```

*QR: ubicación de la decodificación aún no decidida — ver
`docs/CONTEXTO_PROYECTO.md`.

## Principio rector

**Nada crítico para la seguridad del robot depende de la red hacia el operador.** La navegación autónoma corre completa dentro de `robot_ws`, comunicándose con ambos Arduinos por cable — si el enlace WiFi hacia el `dashboard` se cae, el robot sigue evadiendo obstáculos exactamente igual. El WiFi solo transporta lo que el operador *ve* y los comandos que él manda.

Esto no es una preferencia estética: determina qué medio de comunicación usa cada enlace, en orden de qué tan grave sería que fallara.

| Enlace | Medio | Por qué |
|---|---|---|
| Arduino motores ↔ `robot_ws` | Serial (cable) | El más crítico de todos — un mensaje tarde puede significar un choque. Cero tolerancia a latencia o paquetes perdidos. |
| Arduino sensores ↔ `robot_ws` | Serial (cable) | Menos crítico en tiempo real, pero ya está físicamente montado ahí — no hay razón para complicarlo con radio. |
| `robot_ws` ↔ `dashboard` | WiFi | Es el único enlace que *tiene* que ser inalámbrico — son dos laptops en posiciones físicas distintas. Es también, por diseño, el enlace menos crítico para la seguridad. |

## Por qué el enlace WiFi se divide en dos canales

Telemetría (posición, mapa, sensores, comandos) y video comparten el mismo enlace WiFi, pero **no el mismo canal lógico**:

- **`rosbridge_suite`** expone los topics de ROS2 como WebSocket/JSON — liviano, prioritario. Aquí viaja todo lo que es pequeño y crítico, incluyendo un eventual comando de parada de emergencia.
- **`web_video_server`** expone la cámara como stream HTTP (MJPEG) — pesado, tolerante a degradarse.

Si compartieran un solo canal, un frame de video pesado podría retrasar la llegada de un comando de STOP. Separarlos significa que el video puede entrecortarse sin que eso afecte en absoluto la capacidad del operador de detener el robot o ver su posición en el mapa.

## Robustez de la red WiFi

La red del venue es compartida con el resto de los equipos — no hay control sobre cuánta interferencia habrá. Recomendaciones, en orden de prioridad:

1. **Enlace punto a punto dedicado**: la laptop del robot hostea su propio hotspot (no se conecta a la red del venue), la laptop del operador se conecta directo a ese hotspot. Elimina la competencia de ancho de banda con el resto de los equipos. *Pendiente confirmar con organizadores si está permitido.*
2. **IP estática en ambas laptops** (ej. `192.168.4.1` robot, `192.168.4.2` operador) — evita la negociación DHCP en cada reconexión.
3. **Banda 5GHz sobre 2.4GHz** cuando ambos adaptadores lo soporten — significativamente menos saturada en un venue con muchos equipos.
4. **Reconexión automática del lado del `dashboard`**, sin bloquear a `robot_ws` mientras tanto — la navegación nunca debe esperar a que la red vuelva.
5. **Canal de radio dedicado de bajo costo, opcional**: un par NRF24L01/HC-12 (~$150-300 MXN) conectado directo a los Arduinos, exclusivamente para heartbeat + STOP + posición aproximada — un respaldo independiente de cualquier problema de WiFi/espectro, no pensado para video ni telemetría completa.

Probar este esquema con muchos dispositivos WiFi alrededor simulando el caos real del venue, no solo en un ambiente controlado, antes del evento.

## Consideraciones de rendimiento del lado del `dashboard`

Los datos de alta frecuencia (video, posición/mapa) no deben pasar por el ciclo de re-render de React — se dibujan en un `<canvas>` vía `useRef`, sincronizados con `requestAnimationFrame`. Los datos de baja frecuencia (sensores ambientales, estado de conexión) sí usan `useState` normal. El banner de "sin comunicación con el robot" debe ser imposible de ignorar (no un ícono pequeño) — el operador necesita enterarse de inmediato si se pierde el canal de telemetría.

## Ver también

- [`CONTEXTO_PROYECTO.md`](./CONTEXTO_PROYECTO.md) — resumen completo de decisiones y preguntas abiertas de todo el proyecto.
- [`protocols/`](./protocols) — formato exacto de cada mensaje en cada enlace.
- `robot_ws/README.md` — detalle de los paquetes ROS2 y su responsabilidad interna.
