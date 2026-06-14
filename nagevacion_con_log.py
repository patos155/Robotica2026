#--------------------------------------------------
# ESTE CODIGO SOLO PUEDE CORRER EN UBUNTU (LINUX)
#--------------------------------------------------

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Twist
from serial.tools import list_ports
import numpy as np
import serial
import time
import json
import os
import threading
from datetime import datetime
import cv2

class ObstacleAvoidance(Node): 

    def __init__(self):
        super().__init__('obstacle_avoidance')

        # Definicion de parametros que usara el RPLIDAR
        self.declare_parameter('min_distance_fr', 0.6)  # en metros (esta es la medida mas importante ya que es en donde se basa de cuando girar y cuando no)
        self.declare_parameter('max_distance_fr', 1.2)   # en metros
        self.declare_parameter('min_distance_ld', 0.4)  # en metros
        self.declare_parameter('max_distance_ld', 0.6)  # en metros
        self.min_distance_fr = self.get_parameter('min_distance_fr').value
        self.max_distance_fr = self.get_parameter('max_distance_fr').value
        self.min_distance_ld = self.get_parameter('min_distance_ld').value
        self.max_distance_ld = self.get_parameter('max_distance_ld').value
        self.modo = ""

        #Crea el registro de los comandos usados durante el recorrido
        #Crea un archivo llamado "pruebaX.json"
        numeroarchivo = 0
        while os.path.exists(f"prueba{numeroarchivo}.json"):
            numeroarchivo += 1
        self.declare_parameter('log_file',f"prueba{numeroarchivo}.json")
        self.log_file = self.get_parameter('log_file').value
        self._log_lock = threading.Lock()
        print("Archivo:", self.log_file)

        # Almacena los datos de los sensores en el JSON
        self.ultrasonic_data = {
            'MIT': 0,
            'MID': 0,
            'MFI': 0,
            'MFD': 0,
            'MDD': 0,
            'MDT': 0,
        }

        # Se define el estado inicial del arduino
        self.last_cmd = None
        self.last_state = ""

        self.arduino_busy = False
        self.timeout_timer = None

        # Se definen las variables para guardar las distancias del RPLIDAR, para mandarlas al JSON
        self.dist_front = 0.0
        self.dist_left = 0.0
        self.dist_right = 0.0

        # Se hace la conexion con el Arduino mediante el SERIAL
        try:
            self.ser = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)
            time.sleep(2)
            self.ser.reset_input_buffer()
            self.get_logger().info("Serial conectado al Arduino")
            self.serial_thread = threading.Thread(target=self.listen_arduino, daemon=True)
            self.serial_thread.start()

            time.sleep(0.5)
            self.ser.write(b'LIDAR_ON\n')
            self.get_logger().info("Enviado: LIDAR_ON")

        except Exception as e:
            self.get_logger().error(f"Error serial: {e}")
            self.ser = None

        # Se hace la subscripcion a scan, de esta manera sabremos que nos esta mandando el RPLIDAR
        self.scan_subscription = self.create_subscription(
            LaserScan,
            'scan',
            self.scan_callback,
            10
        )

        # Se crea el topico cmd_vel para la comunicacion entre el RPLIDAR y el Arduino
        self.cmd_publisher = self.create_publisher(
            Twist,
            'cmd_vel',
            10
        )

        # Indices de las camaras USB (0 y 1 por defecto, cambiar si es necesario)
        self._cam_index_1 = 0
        self._cam_index_2 = 2
        self._camera_running = True
        self._camera_thread = threading.Thread(
            target=self._camera_loop,
            daemon=True
        )
        self._camera_thread.start()

    # Camaras
    def _camera_loop(self):
        cap1 = cv2.VideoCapture(self._cam_index_1)
        cap2 = cv2.VideoCapture(self._cam_index_2)

        if not cap1.isOpened():
            self.get_logger().error(
                f"No se pudo abrir camara {self._cam_index_1}"
            )
        if not cap2.isOpened():
            self.get_logger().error(
                f"No se pudo abrir camara {self._cam_index_2}"
            )

        while self._camera_running:
            frames = []

            if cap1.isOpened():
                ret1, frame1 = cap1.read()
                if ret1:
                    # Etiqueta en la imagen de la camara 1
                    cv2.putText(
                        frame1, "Camara 1",
                        (10, 30), cv2.FONT_HERSHEY_SIMPLEX,
                        1.0, (0, 255, 0), 2
                    )
                    frames.append(frame1)
                else:
                    # Si falla la lectura, mostrar cuadro negro con aviso
                    blank = np.zeros((480, 640, 3), dtype=np.uint8)
                    cv2.putText(
                        blank, "Camara 1 sin senal",
                        (80, 240), cv2.FONT_HERSHEY_SIMPLEX,
                        1.0, (0, 0, 255), 2
                    )
                    frames.append(blank)
            else:
                blank = np.zeros((480, 640, 3), dtype=np.uint8)
                cv2.putText(
                    blank, "Camara 1 no disponible",
                    (60, 240), cv2.FONT_HERSHEY_SIMPLEX,
                    1.0, (0, 0, 255), 2
                )
                frames.append(blank)

            if cap2.isOpened():
                ret2, frame2 = cap2.read()
                if ret2:
                    # Etiqueta en la imagen de la camara 2
                    cv2.putText(
                        frame2, "Camara 2",
                        (10, 30), cv2.FONT_HERSHEY_SIMPLEX,
                        1.0, (0, 255, 0), 2
                    )
                    frames.append(frame2)
                else:
                    blank = np.zeros((480, 640, 3), dtype=np.uint8)
                    cv2.putText(
                        blank, "Camara 2 sin senal",
                        (80, 240), cv2.FONT_HERSHEY_SIMPLEX,
                        1.0, (0, 0, 255), 2
                    )
                    frames.append(blank)
            else:
                blank = np.zeros((480, 640, 3), dtype=np.uint8)
                cv2.putText(
                    blank, "Camara 2 no disponible",
                    (60, 240), cv2.FONT_HERSHEY_SIMPLEX,
                    1.0, (0, 0, 255), 2
                )
                frames.append(blank)

            # Igualar altura de ambos frames antes de concatenar
            h1, w1 = frames[0].shape[:2]
            h2, w2 = frames[1].shape[:2]
            if h1 != h2:
                # Redimensionar el segundo frame para que coincida con el primero
                frames[1] = cv2.resize(frames[1], (w1, h1))

            # Unir las dos camaras horizontalmente en una sola ventana
            combined = np.hstack(frames)
            cv2.imshow("Camaras del Robot", combined)

            # Salir con 'q' sin cerrar el nodo ROS2
            if cv2.waitKey(1) & 0xFF == ord('q'):
                self.get_logger().info(
                    "Ventana de camaras cerrada por el usuario"
                )
                break

        cap1.release()
        cap2.release()
        cv2.destroyAllWindows()

    # Se definen los sectores del RPLIDAR para guardarlos en el archivo JSON
    def get_sector(self, msg, angle_start_deg, angle_end_deg):
        angle_min = msg.angle_min
        angle_inc = msg.angle_increment
        ranges = np.array(msg.ranges)
        ranges[ranges == 0] = self.max_distance_fr
        ranges[np.isnan(ranges)] = self.max_distance_fr
        ranges[np.isinf(ranges)] = self.max_distance_fr
        angles = angle_min + np.arange(len(ranges)) * angle_inc
        angles_deg = np.degrees(angles)
        
        if angle_start_deg < angle_end_deg:
            mask = ((angles_deg >= angle_start_deg) & (angles_deg <= angle_end_deg))

        else:
            # Cruza 180°
            mask = ((angles_deg >= angle_start_deg) | (angles_deg <= angle_end_deg))

        return ranges[mask]

    # Se escucha al Arduino, de esta manera se mandan los prints que envie el Arduino al JSON
    def listen_arduino(self):

        # Si hay mensajes vacios, no los manda al archivo JSON
        IGNORE_MESSAGES = [
            "------------"
        ]

        while True:
            if self.ser and self.ser.in_waiting > 0:
                try:
                    line = self.ser.readline() \
                        .decode('utf-8') \
                        .strip()

                    # Cuando el Arduino manda una "T" significa que ha acabado la secuencia que estaba ejecutando
                    if line == 'T':

                        self.arduino_busy = False
                        self.last_state = ""
                        self.last_cmd = None

                        if self.timeout_timer:
                            self.timeout_timer.cancel()
                            self.timeout_timer = None

                        self.get_logger().info("Arduino termino secuencia")

                        self.log_arduino_message("FIN_SECUENCIA")
                    
                    # Manda los datos de los sensores al JSON
                    elif line.startswith('{') and line.endswith('}'):
                        try:
                            sensor_data = json.loads(line)

                            if 'GAS_D' in sensor_data:
                                # Datos de gas y temperatura/humedad
                                self.get_logger().info(
                                    f"Gas: digital={sensor_data.get('GAS_D')} "
                                    f"analogico={sensor_data.get('GAS_A')} | "
                                    f"Temp={sensor_data.get('TEMP')}°C "
                                    f"Humedad={sensor_data.get('HUM')}%"
                                )
                            else:
                                # Datos de ultrasonicos
                                self.ultrasonic_data.update(sensor_data)
                                self.print_sensor_data(sensor_data)

                        except json.JSONDecodeError:
                            self.get_logger().warning(f"JSON invalido: {line}")

                    elif line == "CAMBIO A AUTONOMO":
                        print("-----------------Autonomo-----------------")
                        self.modo = "Autonomo"
                        self.last_cmd = None
                        self.last_state = ""
                        self.arduino_busy = False
                        time.sleep(0.3)
                        self.get_logger().info("Modo: Autonomo")
                    elif line == "CAMBIO A MANUAL":
                        print("-----------------Manual-------------------")
                        self.modo="Manual"
                        self.last_cmd = None
                        self.last_state = ""
                        self.arduino_busy = False
                        self.get_logger().info("Modo: Manual")

                    elif line == "Lidar conectado":
                        self.get_logger().info("Arduino confirma: Lidar conectado")

                    elif line == "Se perdio conexion con el Lidar":
                        self.get_logger().warning("Arduino confirma: Se perdio conexion con el Lidar")

                    # Filtra el JSON
                    elif line.startswith("Ch #"):
                        pass

                    elif line in IGNORE_MESSAGES:
                        pass

                    # Cuando llega el print del Arduino, lo manda al JSON
                    elif line:
                        self.get_logger().info(f"Arduino: {line}")
                        self.log_arduino_message(line)

                except Exception as e:

                    self.get_logger().warning(f"Error leyendo Arduino: {e}")

            time.sleep(0.01)

    # imprimir sensores en la terminal
    def print_sensor_data(self, data):

        self.get_logger().info(
            f"Ultrasonicos (cm): "
            f"IT={data.get('MIT', 0)} "
            f"ID={data.get('MID', 0)} "
            f"FI={data.get('MFI', 0)} "
            f"FD={data.get('MFD', 0)} "
            f"DD={data.get('MDD', 0)} "
            f"DT={data.get('MDT', 0)}"
        )

    # Obtiene el valor del sensor
    def get_ultrasonic_value(self, sensor_name):
        return self.ultrasonic_data.get(sensor_name, 0)

    # Se define un timeout por precaucion de que el Arduino se quede "Trabado"
    def reset_busy(self):
        if self.arduino_busy:
            self.get_logger().warning("Timeout - Desbloqueando Arduino")
            self.arduino_busy = False
            self.last_state = ""
            self.timeout_timer = None

    # Envia el comando al Arduino, ya sea F = Adelante, R = Derecha, L = Izquierda, S = Stop, U = Vuelta en U
    def send_command(self, cmd):

        if self.arduino_busy:
            self.get_logger().warning(f"Arduino ocupado - Ignorando: {cmd}")
            return

        if cmd == self.last_cmd and cmd == "F":
            return

        self.last_cmd = cmd
        self.log_direction(cmd,source="send_command")

        if self.ser:

            if cmd in ['L', 'R', 'U']:
                self.arduino_busy = True
                self.get_logger().info(f"Arduino ocupado - Giro {cmd}")
            self.ser.write((cmd + '\n').encode())

        else:

            self.get_logger().warning(f"Arduino NO conectado: {cmd}")

        self.get_logger().info(f"Arduino <- {cmd}")

    # Se mandan al JSON los comandos enviados al Arduino
    def log_direction(self, cmd, source="send_command"):

        if cmd not in ("F", "L", "R", "S", "U"):
            return

        entry = {
            "timestamp": datetime.now().strftime(
                "%Y-%m-%d %H:%M:%S"
            ),
            "type": "command",
            "command": cmd,
            "source": source,
            "lidar": {
                "front": self.dist_front,
                "left": self.dist_left,
                "right": self.dist_right
            },

            "ultrasonic": self.ultrasonic_data.copy()
        }

        self.append_log(entry)

    # Se mandan al JSON los comandos que el Arduino devuelve    
    def log_arduino_message(self, message):

        entry = {
            "timestamp": datetime.now().strftime(
                "%Y-%m-%d %H:%M:%S"
            ),
            "type": "arduino_message",
            "message": message,
            "last_command": self.last_cmd,
            "lidar": {
                "front": self.dist_front,
                "left": self.dist_left,
                "right": self.dist_right
            },
            "ultrasonic": self.ultrasonic_data.copy()
        }

        self.append_log(entry)

    # Escribe el archivo JSON 
    def append_log(self, entry):

        with self._log_lock:
            try:
                data = []

                if os.path.exists(self.log_file):

                    with open(self.log_file, "r") as f:
                        try:
                            data = json.load(f)
                            
                            if not isinstance(data, list):
                                data = []

                        except json.JSONDecodeError:
                            data = []
                data.append(entry)
                tmp_path = self.log_file + ".tmp"

                with open(tmp_path, "w") as f:
                    json.dump(data, f, ensure_ascii=False, indent=2)

                os.replace(tmp_path, self.log_file)

            except Exception as e:
                self.get_logger().warning(f"Error log JSON: {e}")

    # Se definen los sectores de escaneo del RPLIDAR
    def scan_callback(self, msg):

        if self.arduino_busy:
            return

        ranges = np.array(msg.ranges)

        ranges[ranges == 0] = self.max_distance_fr
        ranges[np.isnan(ranges)] = self.max_distance_fr
        ranges[np.isinf(ranges)] = self.max_distance_fr

        # Se definen los sectores de escaneo del RPLIDAR (hay veces que es necesario cambiar los parametros señalados)
        front = self.get_sector(msg, 170, -170) # Frontal
        left = self.get_sector(msg, -100, -80) # Izquierdo
        right = self.get_sector(msg, 80, 100) # Derecho

        # Promedio minimo de escaneo del RPLIDAR
        def avg_min(sector):

            if len(sector) == 0:
                return self.max_distance_fr

            valid = sector[sector > 0.05]

            if len(valid) == 0:
                return self.max_distance_fr

            return np.mean(np.sort(valid)[:3])

        dist_front = avg_min(front)
        dist_left = avg_min(left)
        dist_right = avg_min(right)

        # Guarda las distancias
        self.dist_front = float(dist_front)
        self.dist_left = float(dist_left)
        self.dist_right = float(dist_right)

        # Solo manda comandos cuando esta en modo AUTONOMO
        if self.modo == "Autonomo":
            # Vuelta en U
            if (dist_front < self.min_distance_fr and dist_right < self.min_distance_ld and dist_left < self.min_distance_ld):

                state = (f"Obst DER IZQ FRONTAL " f"({dist_right:.2f}m) - U")

                if state != self.last_state:
                    self.send_command('U')
                    self.get_logger().warn(state)
                    self.last_state = state

            # Obstaculo frontal y se decide para que lado girar, dependiendo de donde este una pared
            elif dist_front < self.min_distance_fr:

                if dist_left > dist_right:

                    state = (f"Obst frontal " f"({dist_front:.2f}m) - IZQ")

                    if state != self.last_state:
                        self.send_command('L')
                        self.get_logger().warn(state)
                        self.last_state = state

                else:

                    state = (f"Obst frontal "f"({dist_front:.2f}m) - DER")

                    if state != self.last_state:
                        self.send_command('R')
                        self.get_logger().warn(state)
                        self.last_state = state

            # Gira a la izquierda si hay un obstaculo a la derecha
            elif dist_left < self.min_distance_ld:

                state = (f"Obst IZQ " f"({dist_left:.2f}m)")

                if state != self.last_state:

                    self.send_command('R')
                    self.get_logger().warn(state)
                    self.last_state = state

            # Gira a la derecha si hay un obstaculo a la izquierda
            elif dist_right < self.min_distance_ld:

                state = (f"Obst DER " f"({dist_right:.2f}m)")

                if state != self.last_state:

                    self.send_command('L')
                    self.get_logger().warn(state)
                    self.last_state = state

            # Camino libre
            else:

                state = (f"Libre " f"F:{dist_front:.2f} " f"L:{dist_left:.2f} " f"R:{dist_right:.2f}")

                if state != self.last_state:
                    self.send_command('F')
                    self.get_logger().info(state)
                    self.last_state = state

    # Cierre del nodo de ROS2
    def destroy_node(self):

        if self.timeout_timer:
            self.timeout_timer.cancel()

        if self.ser:
            time.sleep(0.2)
            self.ser.write(b'S\n')
            self.ser.close()

        super().destroy_node()


# Punto de entrada principal del nodo ROS2
def main(args=None):
    rclpy.init(args=args)
    node = ObstacleAvoidance()
    keyboard_interrupt = False

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        keyboard_interrupt = True
        node.send_command('S')
        if node.ser:
            node.ser.write(b'S\n')

    finally:
        if not keyboard_interrupt:
            if node.ser:
                node.ser.write(b'LIDAR_OFF\n')
        node.destroy_node()
        rclpy.shutdown()


main()