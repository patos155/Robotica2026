"""Utilidades puras para el protocolo serial del Arduino de motores."""

import json

SENSOR_ULTRASONIC = 'ultrasonic'
CMD_FORWARD = 'F'
CMD_LEFT = 'L'
CMD_RIGHT = 'R'
CMD_UTURN = 'U'
CMD_STOP = 'S'
ANGULAR_EPSILON = 0.1
LINEAR_EPSILON = 0.1


def parse_line(line):
    """Regresa el diccionario JSON de una linea del Arduino, o None."""
    text = line.strip() if isinstance(line, str) else ''
    if not text.startswith('{') or not text.endswith('}'):
        return None
    try:
        payload = json.loads(text)
    except (ValueError, TypeError):
        return None
    if not isinstance(payload, dict) or not isinstance(payload.get('type'), str):
        return None
    return payload


def is_ultrasonic(payload):
    """Verifica que el mensaje traiga las distancias de los HC-SR04."""
    return (
        payload.get('type') == 'sensor'
        and payload.get('sensor') == SENSOR_ULTRASONIC
        and isinstance(payload.get('data'), dict)
    )


def twist_to_command(linear_x, angular_z):
    """Traduce un Twist a F/L/R/S, con prioridad para el giro."""
    if angular_z > ANGULAR_EPSILON:
        return CMD_LEFT
    if angular_z < -ANGULAR_EPSILON:
        return CMD_RIGHT
    if linear_x > LINEAR_EPSILON:
        return CMD_FORWARD
    return CMD_STOP


def sensor_record(data, timestamp):
    """Construye el registro JSON de una lectura de ultrasonicos."""
    return {
        'timestamp': timestamp,
        'type': 'sensor',
        'sensor': SENSOR_ULTRASONIC,
        'data': data,
    }


def command_record(command, source, timestamp):
    """Construye el registro JSON de un comando enviado al Arduino."""
    return {
        'timestamp': timestamp,
        'type': 'cmd',
        'command': command,
        'source': source,
    }
