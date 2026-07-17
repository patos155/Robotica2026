# Guía de contribución

Reglas de estilo por lenguaje: `CONVENTIONS.md` de cada componente.
Reglas transversales al repo: [`docs/CONVENTIONS.md`](./docs/CONVENTIONS.md).

## Ramas

- `main` — siempre en estado llevable a competencia. Solo recibe merges desde `dev`.
- `dev` — rama de integración, donde convergen los cambios antes de probarse en el robot.
- Cada desarrollador parte de `dev`, nunca de `main`.

Formato: `<tipo>/<área>-<descripción-corta>` (mismo tipo/área que el commit final).

```
feat/motors-watchdog-desconexion
fix/dashboard-data-view
refactor/motors-serial-comm
docs/protocolo-telemetria        ← si el tipo ya es docs, no repetir el área
```

## Commits — Conventional Commits

Formato: `<tipo>(<área>): Descripción corta en imperativo`

| Tipo | Uso |
|---|---|
| `feat` | Funcionalidad nueva |
| `fix` | Corrección de bug |
| `refactor` | Reorganizar sin cambiar comportamiento |
| `docs` | Solo documentación |
| `chore` | Configuración, dependencias, mantenimiento |

Áreas: `motors`, `sensors`, `robot`, `dashboard`, `docs`.

```
feat(motors): Definición de pines de motores
fix(robot): Reactivar timeout del watchdog en obstacle_avoidance
docs(protocols): Documentar formato de trama del protocolo de sensores
```

Un commit = un cambio lógico. El cuerpo (opcional) explica el *por qué*, no el qué.

## Pull requests

| | Rama → `dev` | `dev` → `main` |
|---|---|---|
| Revisión | Rápida, que compile y tenga sentido | Cuidadosa, checklist completo |
| Prueba física | No necesaria | Sí, integración probada |
| Merge | **Squash** (limpia commits de la rama) | **Normal** (conserva historial ya limpio) |
| Al terminar | Eliminar la rama | Tag si es antes de un evento |

> Squash solo al entrar a `dev` — squashear también al pasar a `main` colapsaría
> semanas de trabajo en un commit y perdería trazabilidad justo donde más importa.
> El mensaje del squash debe editarse a mano para quedar como un commit limpio,
> no como la lista concatenada de commits de la rama.

### Checklist antes de abrir PR

- [ ] Compila/corre sin errores
- [ ] Cambios de protocolo (Serial o telemetría) reflejados en `docs/protocols/`
- [ ] Cambios de comunicación entre componentes reflejados en `docs/ARCHITECTURE.md`
- [ ] Comentarios en español, código en inglés
- [ ] Sin números mágicos — constantes en su lugar designado (`config.h`, `params.yaml`, etc.)
- [ ] Sin `print`/`console.log`/`Serial.println` sueltos fuera del logger de esa área
- [ ] Revisado el `CONVENTIONS.md` del área tocada

Para cambios en un protocolo compartido, que revise alguien que entienda ambos lados del enlace.

## Antes de un evento

- [ ] Tag del commit: `git tag <evento>-v1` (ej. `clasificatoria-marzo2027-v1`)
- [ ] `dashboard` con build de producción (`npm run build`), no modo desarrollo
- [ ] Firmwares subidos a los Arduinos coinciden con `main`
