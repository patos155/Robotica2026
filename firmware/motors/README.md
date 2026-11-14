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
├── README.md                ← este archivo
├── CONVENTIONS.md           ← convenciones específicas de este firmware
└── movement/                ← sketch de Arduino (todavía sin platformio.ini)
    ├── movement.ino         ← setup()/loop() — orquestación + checkWatchdog()
    ├── config.h             ← pines, constantes, umbrales, DEBUG_MODE
    ├── protocol.h           ← contrato Serial — sincronizado con
    │                           docs/protocols/motors-protocol.md
    └── src/
        ├── motors/                 Motors — único que escribe pines de motor
        ├── sensors/Ultrasonic/     UltrasonicArray — sin dependencias del proyecto
        ├── maneuvers/              Maneuvers — único con esperas largas, abortables con el control
        ├── communication/          Communication — único que escribe/lee Serial
        ├── remote/                 Remote — solo reporta estado, no decide
        └── system/                 Logger — INFO/WARNING/ERROR/DEBUG
```

Ver `CONVENTIONS.md` para el grafo de dependencias, reglas de arquitectura y estilo de código.

## Protocolo Serial

JSON por línea (`\n`), campo `"type"` obligatorio: `cmd` (`F`/`L`/`R`/`U`/`S`), `status` (`DONE`), `mode`, `sensor` (solo `"ultrasonic"`, los 6 HC-SR04) y `log`. Contrato completo en `docs/protocols/motors-protocol.md` — cualquier cambio aquí debe reflejarse ahí en el mismo PR.

Mientras no exista `motors_bridge`, `LEGACY_PROTOCOL = true` en `config.h` cambia a texto plano para `nagevacion_legacy.py`: comandos `F`/`L`/`R`/`U`/`S` sin JSON, `T` al terminar cada maniobra, `LOG: ...` y `MODE: ...`. Detalle en la sección "Modo legacy" del contrato.

> Las lecturas de gas y humedad pertenecen al protocolo de
> `firmware/sensors`, no a este.

## Watchdog de seguridad

En modo autónomo, si no llega un comando nuevo en 500 ms (constante `CMD_TIMEOUT_MS` en `config.h`), los motores se detienen solos y se manda un log. Requisito no negociable — visible como `checkWatchdog()` en `movement.ino`, llamado en cada iteración de `loop()` del modo autónomo.

Con `LEGACY_PROTOCOL = true` está desactivado: `nagevacion_legacy.py` manda `F` una sola vez y el watchdog detendría el robot a medio segundo de cada `F`. La razón y el trade-off están en `config.h`.

## Estado

- [x] Estructura de módulos y convenciones definidas
- [x] Watchdog implementado (`checkWatchdog()`, 500 ms; desactivado con `LEGACY_PROTOCOL`)
- [x] Que compile (Arduino Mega 2560, sin errores ni warnings con `-Wall`)
- [ ] Migración a PlatformIO
- [ ] Confirmar modelo de placa física