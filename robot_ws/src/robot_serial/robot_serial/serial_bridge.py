"""Nodo puente entre el Arduino de motores y ROS2."""

import json
import os
import time
from datetime import datetime, timezone

import rclpy
from geometry_msgs.msg import Twist
from rclpy.node import Node
from serial import Serial, SerialException
from std_msgs.msg import String

from robot_serial.serial_protocol import (
    CMD_FORWARD,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_STOP,
    CMD_UTURN,
    command_record,
    is_ultrasonic,
    parse_line,
    sensor_record,
    twist_to_command,
)

DEFAULT_SERIAL_PORT = '/dev/ttyUSB1'
DEFAULT_BAUD_RATE = 9600
DEFAULT_LOG_FILE = '~/.ros/robot_logs/ultrasonic.json'
DEFAULT_COMMAND_RATE_HZ = 5.0
DEFAULT_MANEUVER_TIMEOUT_S = 15.0
SERIAL_READ_INTERVAL_S = 0.02
SERIAL_RETRY_INTERVAL_S = 1.0
SERIAL_OPEN_DELAY_S = 0.2
FLUSH_INTERVAL_S = 10.0
MAX_RX_CHUNK = 4096

MODE_AUTO = 'AUTO'
MODE_MANUAL = 'MANUAL'
MANEUVER_COMMANDS = (CMD_LEFT, CMD_RIGHT, CMD_UTURN)
COMMAND_TEMPLATE = '{"type": "cmd", "value": "%s"}'
CMD_TOPIC = '/cmd_vel'
ULTRASONIC_TOPIC = '/robot/ultrasonic'


class SerialBridge(Node):
    """Lee telemetria del Arduino, guarda el JSON y reenvia comandos."""

    def __init__(self):
        super().__init__('serial_bridge')
        self.declare_parameter('serial_port', DEFAULT_SERIAL_PORT)
        self.declare_parameter('baud_rate', DEFAULT_BAUD_RATE)
        self.declare_parameter('log_file', DEFAULT_LOG_FILE)
        self.declare_parameter('command_rate_hz', DEFAULT_COMMAND_RATE_HZ)
        self.declare_parameter('maneuver_timeout_s', DEFAULT_MANEUVER_TIMEOUT_S)

        self._port = self.get_parameter('serial_port').value
        self._baud = int(self.get_parameter('baud_rate').value)
        self._log_path = os.path.expanduser(self.get_parameter('log_file').value)
        rate = max(0.1, float(self.get_parameter('command_rate_hz').value))
        self._maneuver_timeout_s = float(
            self.get_parameter('maneuver_timeout_s').value)

        self._serial = None
        self._rx_buffer = b''
        self._last_retry = 0.0
        self._records = []
        self._desired_command = CMD_STOP
        self._mode = None
        self._busy = False
        self._busy_since = None
        self._last_maneuver = None

        self._open_serial()
        self.create_subscription(Twist, CMD_TOPIC, self.on_cmd_vel, 10)
        self.telemetry_publisher = self.create_publisher(
            String, ULTRASONIC_TOPIC, 10)
        self.create_timer(SERIAL_READ_INTERVAL_S, self.on_serial_read)
        self.create_timer(1.0 / rate, self.on_send_command)
        self.create_timer(FLUSH_INTERVAL_S, self.flush_records)
        self.get_logger().info('serial_bridge iniciado')

    def _open_serial(self):
        """Abre el puerto y descarta lo que quedo del Arduino al reiniciar."""
        try:
            self._serial = Serial(
                port=self._port, baudrate=self._baud, timeout=0)
            time.sleep(SERIAL_OPEN_DELAY_S)
            self._serial.reset_input_buffer()
            self._rx_buffer = b''
            self.get_logger().info(f'Serial conectado: {self._port}')
        except (SerialException, OSError) as exc:
            self._serial = None
            self._last_retry = time.monotonic()
            self.get_logger().error(f'No se pudo abrir {self._port}: {exc}')

    def _close_serial(self, reason):
        """Cierra el puerto y programa el siguiente intento de conexion."""
        if self._serial is not None:
            try:
                self._serial.close()
            except SerialException:
                pass
        self._serial = None
        self._rx_buffer = b''
        self._last_retry = time.monotonic()
        self.get_logger().warning(f'Serial desconectada: {reason}')

    def on_serial_read(self):
        """Junta los bytes disponibles y procesa las lineas completas."""
        if self._serial is None:
            now = time.monotonic()
            if now - self._last_retry >= SERIAL_RETRY_INTERVAL_S:
                self._open_serial()
            return
        try:
            count = self._serial.in_waiting
            if count:
                self._rx_buffer += self._serial.read(count)
        except (SerialException, OSError) as exc:
            self._close_serial(exc)
            return
        if len(self._rx_buffer) > MAX_RX_CHUNK:
            self._rx_buffer = self._rx_buffer[-MAX_RX_CHUNK:]
        while b'\n' in self._rx_buffer:
            raw, self._rx_buffer = self._rx_buffer.split(b'\n', 1)
            line = raw.decode('utf-8', errors='ignore').strip()
            payload = parse_line(line)
            if payload is not None:
                self.handle_message(payload, line)

    def handle_message(self, payload, line):
        """Clasifica status, mode, log y lecturas de ultrasonicos."""
        kind = payload['type']
        value = payload.get('value')
        if kind == 'sensor' and is_ultrasonic(payload):
            self._records.append(
                sensor_record(payload['data'], self.timestamp()))
            message = String()
            message.data = line
            self.telemetry_publisher.publish(message)
        elif kind == 'status' and value == 'DONE':
            self._busy = False
            self._busy_since = None
        elif kind == 'mode' and value in (MODE_AUTO, MODE_MANUAL):
            if self._mode != value:
                self.get_logger().info(f'Modo Arduino: {value}')
            self._mode = value
            if value == MODE_MANUAL:
                self._busy = False
                self._busy_since = None
        elif kind == 'log' and isinstance(value, str):
            self.get_logger().info(f'Arduino: {value}')

    @staticmethod
    def timestamp():
        """Regresa la hora actual en ISO 8601 con zona UTC."""
        return datetime.now(timezone.utc).isoformat(
            timespec='milliseconds').replace('+00:00', 'Z')

    def on_cmd_vel(self, msg):
        """Guarda el ultimo Twist recibido como comando deseado."""
        self._desired_command = twist_to_command(msg.linear.x, msg.angular.z)

    def on_send_command(self):
        """Manda el comando vigente alimentando el watchdog del Arduino."""
        if self._serial is None or self._mode != MODE_AUTO:
            return
        now = time.monotonic()
        if self._busy:
            if now - self._busy_since <= self._maneuver_timeout_s:
                return
            self.get_logger().warning('Timeout de maniobra, desbloqueando')
            self._busy = False
            self._busy_since = None
        command = self._desired_command
        if command in MANEUVER_COMMANDS:
            if command == self._last_maneuver:
                return
            self._last_maneuver = command
            self._busy = True
            self._busy_since = now
        else:
            self._last_maneuver = None
        try:
            self._serial.write(
                (COMMAND_TEMPLATE % command + '\n').encode('utf-8'))
        except (SerialException, OSError) as exc:
            self._close_serial(exc)
            return
        self._records.append(
            command_record(command, CMD_TOPIC, self.timestamp()))

    def _load_records(self):
        """Lee el historial JSON, o empieza uno nuevo si esta corrupto."""
        if not os.path.exists(self._log_path):
            return []
        try:
            with open(self._log_path, 'r', encoding='utf-8') as stream:
                data = json.load(stream)
        except (OSError, ValueError) as exc:
            self.get_logger().warning(f'JSON previo invalido: {exc}')
            return []
        if isinstance(data, list):
            return data
        self.get_logger().warning('El JSON previo no es una lista')
        return []

    def flush_records(self):
        """Escribe los registros pendientes con reemplazo atomico."""
        if not self._records:
            return
        directory = os.path.dirname(self._log_path)
        try:
            if directory:
                os.makedirs(directory, exist_ok=True)
            data = self._load_records()
            data.extend(self._records)
            tmp_path = self._log_path + '.tmp'
            with open(tmp_path, 'w', encoding='utf-8') as stream:
                json.dump(data, stream, ensure_ascii=False, indent=2)
            os.replace(tmp_path, self._log_path)
            self._records.clear()
        except OSError as exc:
            self.get_logger().warning(f'No se pudo guardar el JSON: {exc}')

    def destroy_node(self):
        """Volca el JSON, manda S al Arduino y cierra el puerto."""
        self.flush_records()
        if self._serial is not None:
            try:
                self._serial.write(
                    (COMMAND_TEMPLATE % CMD_STOP + '\n').encode('utf-8'))
                self._serial.close()
            except (SerialException, OSError):
                pass
            self._serial = None
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = SerialBridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
