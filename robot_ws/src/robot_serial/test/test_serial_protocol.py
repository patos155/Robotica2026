from robot_serial.serial_protocol import (
    CMD_FORWARD,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_STOP,
    command_record,
    is_ultrasonic,
    parse_line,
    sensor_record,
    twist_to_command,
)

SENSOR_LINE = (
    '{"type": "sensor", "sensor": "ultrasonic", "data": '
    '{"distFL": 45, "distFR": 48, "distLF": 30, "distLR": 32, '
    '"distRF": 28, "distRR": 31}}'
)


def test_parse_line_returns_the_sensor_payload():
    payload = parse_line(SENSOR_LINE + '\r\n')
    assert payload['type'] == 'sensor'
    assert payload['sensor'] == 'ultrasonic'
    assert payload['data']['distFL'] == 45


def test_parse_line_ignores_legacy_and_malformed_lines():
    assert parse_line('T') is None
    assert parse_line('LOG: Hardware listo') is None
    assert parse_line('{"type": "sensor"') is None
    assert parse_line('[1, 2, 3]') is None
    assert parse_line('') is None


def test_is_ultrasonic_requires_a_data_object():
    payload = parse_line(SENSOR_LINE)
    assert is_ultrasonic(payload)
    assert not is_ultrasonic({'type': 'sensor', 'sensor': 'magnet'})
    assert not is_ultrasonic({'type': 'status', 'value': 'DONE'})


def test_twist_to_command_prioritizes_turning():
    assert twist_to_command(1.0, 0.0) == CMD_FORWARD
    assert twist_to_command(0.0, 0.5) == CMD_LEFT
    assert twist_to_command(0.0, -0.5) == CMD_RIGHT
    assert twist_to_command(1.0, 0.5) == CMD_LEFT
    assert twist_to_command(0.0, 0.0) == CMD_STOP


def test_records_keep_the_expected_shape():
    reading = sensor_record({'distFL': 40}, '2026-11-27T12:00:00.000Z')
    assert reading['type'] == 'sensor'
    assert reading['sensor'] == 'ultrasonic'
    assert reading['data']['distFL'] == 40
    command = command_record(CMD_FORWARD, '/cmd_vel', 'ts')
    assert command['type'] == 'cmd'
    assert command['command'] == CMD_FORWARD
    assert command['source'] == '/cmd_vel'
