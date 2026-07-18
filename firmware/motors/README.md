# Firmware — Motors

Firmware del Arduino encargado de motores, ultrasónicos, control remoto y ejecución de maniobras. Parte del proyecto **Robot de Rescate TMR 2027**.

## Rol dentro del sistema

Corre en tiempo real (~10-20Hz) y no tolera retraso. Es el único componente que mueve físicamente el robot. Recibe comandos por Serial desde `robot_ws` (laptop con ROS2 + LiDAR) y ejecuta la maniobra, usando los ultrasonidos como seguridad local ante obstáculos inmediatos.

**Nada crítico para la seguridad del robot depende de la red hacia el operador.** Si el WiFi al `dashboard` se cae, este firmware sigue funcionando sin problema — solo depende del Serial con `robot_ws`.

## Hardware

| Componente | Uso |
|---|---|
| Arduino (confirmar modelo) | Microcontrolador — motores y sensores |
| 4× Motores DC + puente H + relevadores | Locomoción |
| 6× HC-SR04 | Ultrasonidos perimetrales — seguridad local |
| Receptor RC (6 canales) | Control remoto manual |

## Estructura

```
firmware/motors/
├── platformio.ini
├── README.md               ← este archivo
├── CONVENTIONS.md           ← convenciones específicas de este firmware
└── src/
    ├── robot_rescate.ino    ← setup()/loop() — orquestación + verificarWatchdog()
    ├── config.h             ← pines, constantes, umbrales, DEBUG_MODE
    ├── protocol.h            ← contrato Serial — sincronizado con
    │                            docs/protocols/motors-protocol.md
    ├── motors/               MotorController — único que escribe pines de motor
    ├── sensors/               UltrasonicArray — sin dependencias del proyecto
    ├── maneuvers/             Maneuvers — único lugar permitido usar delay()
    ├── communication/         SerialComm — único que escribe/lee Serial (con Logger)
    ├── remote/                RemoteControl — solo reporta estado, no decide
    └── system/                Logger — INFO/WARNING/ERROR/DEBUG
```

Ver `CONVENTIONS.md` para el grafo de dependencias, reglas de arquitectura y estilo de código.

## Protocolo Serial

JSON por línea (`\n`), campo `"type"` obligatorio: `cmd`, `status`, `mode`, `sensor` (solo `"ultrasonic"`), `lidar`, `log`. Contrato completo en `docs/protocols/motors-protocol.md` — cualquier cambio aquí debe reflejarse ahí en el mismo PR.

> Las lecturas de gas y humedad pertenecen al protocolo de
> `firmware/sensors`, no a este.

## Watchdog de seguridad

Si no llega un comando nuevo en ~300–500ms, los motores deben detenerse solos. Requisito no negociable — visible como `verificarWatchdog()` en `robot_rescate.ino`, llamado en cada iteración de `loop()`.

## Estado

- [x] Estructura de módulos y convenciones definidas
- [ ] Migración a PlatformIO
- [ ] Watchdog implementado
- [ ] Confirmar modelo de placa física