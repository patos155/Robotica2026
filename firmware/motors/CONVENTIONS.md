# Convenciones y buenas prácticas

Guía de estilo y decisiones técnicas del equipo para el proyecto Robot de Rescate.
El objetivo es que cualquier integrante pueda leer, entender y modificar cualquier
parte del código sin necesidad de preguntar al autor original.

## Tabla de contenidos
- [Idioma](#idioma)
- [Nomenclatura](#nomenclatura)
- [Tipos de datos](#tipos-de-datos)
- [Constantes](#constantes)
- [Comentarios](#comentarios)
- [Estructura de archivos](#estructura-de-archivos)
- [Reglas de arquitectura](#reglas-de-arquitectura)
- [Comunicación Serial](#comunicación-serial)
- [Manejo de hardware](#manejo-de-hardware)
- [Qué evitar](#qué-evitar)

---

## Idioma

| Elemento | Idioma |
|---|---|
| Variables, funciones, clases, archivos | Inglés |
| Comentarios | Español |
| Documentación (README, CONVENTIONS) | Español |

La razón de separar código en inglés y comentarios en español es que el inglés
es el estándar universal en programación (librerías, ejemplos, foros), mientras
que los comentarios son comunicación interna del equipo.

```cpp
// Bien
void readSensor();           // lee el sensor ultrasónico
bool isObstacleDetected;     // true si hay obstáculo dentro del umbral

// Mal
void leerSensor();
bool obstaculoDetectado;
```

---

## Nomenclatura

### Variables y funciones — `camelCase`

```cpp
int sensorDistance;
bool isObstacleDetected;
void readSensor();
void executeTurn();
```

### Clases — `PascalCase`

```cpp
class MotorController { ... };
class UltrasonicArray { ... };
```

### Constantes — `UPPER_SNAKE_CASE`

```cpp
constexpr uint8_t  PWM_FORWARD           = 150;
constexpr uint16_t T_TURN_MS             = 1500;
constexpr uint8_t  DIST_FRONT_THRESHOLD_CM = 25;
```

### Pines — `UPPER_SNAKE_CASE` con prefijo `PIN_`

```cpp
constexpr uint8_t PIN_TRIG_FL = 29;
constexpr uint8_t PIN_PWM_IZQ = 6;
```

### Archivos — `PascalCase` para clases, `camelCase` para los demás

```
MotorController.h / MotorController.cpp   ← clase
UltrasonicArray.h / UltrasonicArray.cpp  ← clase
config.h                                  ← configuración
protocol.h                                ← protocolo
```

### Booleanos — prefijo `is` o `has`

```cpp
bool isObstacleDetected;
bool hasFinished;
bool isAutonomous;
```

### Atributos privados de clase — prefijo `_`

```cpp
class UltrasonicArray {
private:
  long _distFL;       // distancia frente izquierdo
  bool _hasObstacle;
};
```

El prefijo `_` comunica visualmente que esa variable es interna a la clase
y no debe accederse desde fuera.

---

## Tipos de datos

Usar tipos de ancho fijo en lugar de `int` genérico cuando el tamaño importa:

| Tipo | Rango | Usar para |
|---|---|---|
| `uint8_t` | 0 – 255 | Pines, valores PWM, contadores pequeños |
| `uint16_t` | 0 – 65,535 | Tiempos en ms, distancias, pulsos RC |
| `uint32_t` | 0 – 4,294,967,295 | Tiempos en µs, valores de `millis()` y `micros()` |
| `long` | −2,147,483,648 – 2,147,483,647 | Retorno de `pulseIn()` — requerido por la API de Arduino |
| `float` | decimal | Mapeo de canales RC, cálculos de distancia con decimales |
| `bool` | true / false | Flags lógicos — nunca usar `int` para esto |

```cpp
// Bien
constexpr uint8_t  PIN_TRIG_FL  = 29;    // pin cabe en 8 bits
constexpr uint16_t T_TURN_MS    = 1500;  // tiempo cabe en 16 bits
constexpr uint32_t TIMEOUT_US   = 30000; // timeout en µs — usar 32 bits
bool isObstacle = false;                 // flag lógico → bool
long duration   = pulseIn(...);          // pulseIn() retorna long

// Mal
int pin = 29;          // int ocupa 16 bits innecesariamente
int isObstacle = 0;    // 0/1 no comunica que es un booleano
```

Los literales `float` siempre llevan sufijo `f` para evitar advertencias del compilador:

```cpp
constexpr float RC_DEAD_ZONE_LOW = 0.35f;   // bien
constexpr float RC_DEAD_ZONE_LOW = 0.35;    // mal — es double por defecto
```

---

## Constantes

Usar siempre `constexpr` en lugar de `#define`. El `#define` es una sustitución
de texto sin tipo, sin scope, y el compilador no puede ayudar a detectar errores.

```cpp
// Bien — tiene tipo, scope y el compilador la valida
constexpr uint8_t MAX_SPEED = 255;

// Mal — sustitución de texto ciega, sin tipo ni scope
#define MAX_SPEED 255
```

Todas las constantes del proyecto viven en `config.h`. Nunca declarar constantes
de hardware directamente en los archivos donde se usan:

```cpp
// Bien — la constante está en config.h y se importa
#include "../config.h"
analogWrite(PIN_PWM_IZQ, PWM_FORWARD);

// Mal — número mágico directo en el código
analogWrite(6, 150);
```

---

## Comentarios

Los comentarios explican el **por qué**, no el qué. El código bien escrito
ya dice qué hace — el comentario aporta el contexto que el código no puede dar.

```cpp
// Bien — explica la razón del umbral y su implicación
constexpr uint8_t ALIGN_TOLERANCE_CM = 2;
// si la diferencia entre sensores simétricos es menor a 2cm,
// el robot se considera paralelo al pasillo

// Mal — el código ya lo dice
constexpr uint8_t ALIGN_TOLERANCE_CM = 2; // tolerancia de alineación en cm
```

```cpp
// Bien — explica por qué se detiene el PWM antes que los relevadores
void MotorController::stop() {
  // cortar corriente antes de cambiar relevadores evita picos que dañan el puente H
  analogWrite(PIN_PWM_IZQ, 0);
  analogWrite(PIN_PWM_DER, 0);
  digitalWrite(PIN_MOT_IZQ_A, LOW);
  ...
}

// Mal — comenta lo obvio
void MotorController::stop() {
  analogWrite(PIN_PWM_IZQ, 0);  // pone PWM izquierdo en 0
  analogWrite(PIN_PWM_DER, 0);  // pone PWM derecho en 0
}
```

### Encabezado de cada archivo `.h`

```cpp
// =============================================================
// NombreDeClase
// Descripción breve del propósito del módulo.
// Restricciones importantes (ej: único módulo que toca pines X).
// =============================================================
```

### Documentación de funciones

Solo documentar lo que no es evidente por el nombre y los parámetros:

```cpp
// Bien — nombre autoexplicativo, sin comentario necesario
void forward();
void stop();

// Bien — el parámetro necesita contexto
void setLeft(uint8_t speed, uint8_t direction);
// speed: 0–255 | direction: MOT_FORWARD o MOT_BACKWARD (definidos en config.h)

// Bien — el retorno necesita contexto
long _measure(uint8_t trigPin, uint8_t echoPin) const;
// retorna la distancia en cm, o -1 si la lectura supera el timeout
```

---

## Estructura de archivos

### Orden dentro de un `.h`

```cpp
#pragma once

#include <Arduino.h>        // 1. includes de sistema
#include "OtraClase.h"      // 2. includes de dependencias del proyecto

// 3. encabezado del módulo

class NombreClase {
public:
  void begin();             // 4. métodos públicos — primero begin()
  void update();            //    luego los de uso frecuente en loop()
  void metodoPublico();     //    luego el resto

private:
  int _atributo;            // 5. atributos privados
  void _metodoPrivado();    // 6. métodos privados
};
```

### Orden dentro de un `.cpp`

```cpp
#include "NombreClase.h"    // 1. include del header propio
#include "../config.h"      // 2. includes de dependencias

// 3. implementación en el mismo orden que las declaraciones del .h
void NombreClase::begin() { ... }
void NombreClase::update() { ... }
void NombreClase::metodoPublico() { ... }
void NombreClase::_metodoPrivado() { ... }
```

### `robot_rescate.ino` — solo orquestación

`setup()` y `loop()` no contienen lógica — solo crean objetos, llaman `begin()`
e invocan `update()` de cada módulo:

```cpp
void loop() {
  remote.update();

  if (remote.isAutonomous()) {
    ultrasonics.update();
    comm.update();
    comm.sendSensorData(ultrasonics);
  } else {
    remote.driveMotors(motors);
  }
}
```

---

## Reglas de arquitectura

Estas reglas definen qué módulo puede hacer qué. Violarlas introduce
dependencias ocultas que hacen el código difícil de mantener y depurar.

| Regla | Razón |
|---|---|
| Solo `MotorController` escribe en pines de motor | Cambiar el hardware afecta un solo archivo |
| Solo `SerialComm` escribe en `Serial` | El protocolo vive en un solo lugar |
| Solo `SerialComm` lee comandos entrantes | La lógica de parseo no se dispersa |
| `loop()` no contiene lógica de negocio | El flujo principal es legible de un vistazo |
| `config.h` es el único lugar con números de pin | Reasignar un pin afecta un solo archivo |

### Dependencias permitidas entre módulos

```
robot_rescate.ino
       │
       ├── SerialComm       usa ──► Maneuvers, MotorController
       ├── RemoteControl    usa ──► MotorController
       ├── Maneuvers        usa ──► MotorController, UltrasonicArray
       └── UltrasonicArray  (sin dependencias de otros módulos)
           MotorController  (sin dependencias de otros módulos)
```

`UltrasonicArray` y `MotorController` son módulos base — no dependen de nada
del proyecto, solo de `config.h`. Esto permite probarlos de forma aislada.

---

## Comunicación Serial

Todo mensaje Serial sigue el formato JSON definido en `protocol.h`.
Nunca escribir strings directamente en `Serial` fuera de `SerialComm`:

```cpp
// Bien — usa las constantes del protocolo desde SerialComm
Serial.println(F("{\"type\":\"log\",\"value\":\"Sistema iniciado\"}"));

// Mal — string hardcodeado fuera de SerialComm
Serial.println("Sistema iniciado");
```

Para mensajes de debug usar el tipo `"log"` del protocolo, no `Serial.println()`
directo. Esto permite que Python los reciba, identifique y loguee correctamente
sin confundirlos con datos de sensores o comandos.

La macro `F()` es obligatoria para todos los strings en `Serial.print/println`.
Mueve el string a la memoria Flash y libera RAM — en el Arduino Mega con 8KB
de RAM, cada string sin `F()` compite con las variables del programa:

```cpp
Serial.println(F("Sistema iniciado"));   // bien — string en Flash
Serial.println("Sistema iniciado");      // mal — string en RAM
```

---

## Manejo de hardware

### Inicialización

Nunca inicializar hardware en constructores de clase — el microcontrolador
no está listo en ese momento. Usar siempre `begin()` llamado desde `setup()`:

```cpp
// Bien
void MotorController::begin() {
  pinMode(PIN_MOT_IZQ_A, OUTPUT);  // setup() ya ejecutó, hardware listo
}

// Mal
MotorController::MotorController() {
  pinMode(PIN_MOT_IZQ_A, OUTPUT);  // demasiado temprano, puede fallar
}
```

### Dirección de motores

La dirección física depende del cableado del puente H. Nunca hardcodear
`HIGH`/`LOW` para dirección — usar las constantes `MOT_FORWARD` y `MOT_BACKWARD`
definidas en `config.h`. Si un motor gira al revés en pruebas, solo se cambia
la constante:

```cpp
// Bien
applyLeft(PIN_MOT_IZQ_A, PIN_MOT_IZQ_B, PIN_PWM_IZQ, PWM_FORWARD, MOT_FORWARD);

// Mal
digitalWrite(PIN_MOT_IZQ_A, HIGH);
digitalWrite(PIN_MOT_IZQ_B, LOW);
```

### `delay()` — uso restringido

`delay()` bloquea completamente el microcontrolador — durante ese tiempo no se
leen sensores, no se reciben comandos ni se detectan obstáculos.

- Permitido **solo en `Maneuvers`** para los tiempos de giro
- Con la intención de reemplazarlo por `millis()` si las pruebas en superficies
  irregulares muestran problemas de alineación
- **Nunca** en `loop()`, `update()` de sensores, ni en `SerialComm`

```cpp
// Permitido — dentro de Maneuvers, tiempo definido en config.h
delay(T_TURN_MS);

// No permitido — en cualquier otro módulo
delay(1000);
```

### Lecturas de ultrasonidos fuera de rango

`_measure()` retorna `-1` cuando el sensor no recibe eco dentro del timeout.
Siempre verificar antes de usar la lectura:

```cpp
// Bien — filtra lecturas inválidas
if (dist > 0 && dist < DIST_FRONT_THRESHOLD_CM) { ... }

// Mal — -1 podría interpretarse como obstáculo
if (dist < DIST_FRONT_THRESHOLD_CM) { ... }
```

---

## Qué evitar

| Evitar | Alternativa |
|---|---|
| `#define` para constantes | `constexpr` |
| `int` para flags lógicos | `bool` |
| `int` donde el tamaño importa | `uint8_t`, `uint16_t`, etc. |
| Números mágicos en el código | Constantes con nombre en `config.h` |
| `delay()` fuera de `Maneuvers` | `millis()` para timing no bloqueante |
| `Serial.println()` fuera de `SerialComm` | Métodos de `SerialComm` |
| `digitalWrite` en pines de motor fuera de `MotorController` | Métodos de `MotorController` |
| Strings sin `F()` en `Serial.print` | `Serial.print(F("..."))` |
| `or` / `and` en lugar de operadores estándar | `\|\|` / `&&` |
| Constructores que inicializan hardware | `begin()` llamado desde `setup()` |
EOF