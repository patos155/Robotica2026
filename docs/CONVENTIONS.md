# Convenciones transversales

Reglas que aplican a los 4 componentes del repo, sin importar el lenguaje en que estén escritos. Las reglas específicas de cada lenguaje (nomenclatura, tipos de datos, estilo) viven en el `CONVENTIONS.md` de cada componente.

## Idioma

| Elemento | Idioma |
|---|---|
| Variables, funciones, clases, archivos, nombres de topics/nodos | Inglés |
| Comentarios | Español |
| Documentación (READMEs, este archivo, protocolos) | Español |

## Un solo escritor por sección de estado

Cualquier dato compartido entre partes de un mismo componente (una variable de estado, un topic de ROS2, un archivo) debe tener exactamente **un dueño que lo escribe**. Los demás solo leen. Esto aplica igual a un `shared_state` protegido por lock en Python, a un topic de ROS2 (donde ya es la norma: un publisher, varios subscribers), o a qué módulo tiene permiso de escribir en `Serial` en un Arduino.

Si dos partes necesitan escribir el mismo dato, es señal de que la responsabilidad no está bien dividida — hay que encontrar cuál de las dos debería ser la dueña real.

## Nada crítico depende de la red

La navegación autónoma del robot nunca debe depender de que el enlace WiFi hacia el operador esté funcionando. Cualquier decisión que afecte directamente la seguridad física del robot (evasión de obstáculos, parada de motores) debe poder tomarse con la información que ya está disponible localmente (Serial, sensores propios), no con datos que tuvieron que viajar por la red. Ver [`ARCHITECTURE.md`](./ARCHITECTURE.md) para el razonamiento completo.

## Logging, no `print`/`console.log` sueltos

Cada componente tiene su propio mecanismo de logging estructurado, y todo mensaje de depuración debe pasar por ahí, no por la salida cruda del lenguaje:

| Componente | Mecanismo |
|---|---|
| `firmware/motors`, `firmware/sensors` | Módulo `Logger` propio, niveles INFO/WARNING/ERROR/DEBUG |
| `robot_ws` | Logger nativo de ROS2 (`self.get_logger()`) |
| `dashboard` | Consola del navegador para desarrollo; el `ConnectionBanner` para alertas que el operador debe ver sí o sí |

La razón: un log dentro del mecanismo estructurado se puede filtrar, desactivar en producción, o transmitir al otro lado del enlace sin confundirse con datos reales — un `print`/`console.log` suelto no.

## Sincronización de protocolos

Cualquier formato de mensaje que cruce un enlace entre dos componentes (Serial o telemetría) tiene **una sola fuente de verdad**: el archivo correspondiente en [`protocols/`](./protocols). Un cambio al protocolo se hace en el mismo PR que actualiza ambos lados del enlace — nunca se cambia un lado confiando en actualizar el otro "después". Ver el checklist de PR en `CONTRIBUTING.md`.

## Qué evitar en cualquier componente

| Evitar | Razón |
|---|---|
| Números mágicos sin nombre | Dificulta saber por qué ese valor específico |
| Un módulo que hace más de una cosa a la vez | Dificulta probar y depurar por separado |
| Bloquear un hilo/callback crítico con I/O lento (red, disco) | El mismo motivo por el que nada crítico depende de la red |
| Lógica de negocio en el punto de orquestación (`loop()`, `main()`, launch file) | El punto de entrada debe ser legible de un vistazo, no esconder decisiones |
| Agregar una herramienta/librería que no reduce trabajo real | Ver el criterio de sobreingeniería aplicado al elegir React sin Redux/Router/UI kit pesado |