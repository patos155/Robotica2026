import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Twist
import numpy as np
import serial
import time
import json
import os
from datetime import datetime
import threading

class ObstacleAvoidance(Node):
    def __init__(self):
        super().__init__('obstacle_avoidance')

        # ================= PARAMETROS (en metros) =================
        self.declare_parameter('min_distance_fr', 0.85)
        self.declare_parameter('max_distance_fr', 0.9)
        self.declare_parameter('min_distance_ld', 0.6)
        self.declare_parameter('max_distance_ld', 0.7)

        self.min_distance_fr = self.get_parameter('min_distance_fr').value
        self.max_distance_fr = self.get_parameter('max_distance_fr').value
        self.min_distance_ld = self.get_parameter('min_distance_ld').value
        self.max_distance_ld = self.get_parameter('max_distance_ld').value

        # ================= LOG JSON =================
        self.declare_parameter('log_file', 'movement_log.json')
        self.log_file = self.get_parameter('log_file').value
        self._log_lock = threading.Lock()

        if os.path.exists(self.log_file):
            try:
                os.remove(self.log_file)
                self.get_logger().info(f"Log anterior eliminado: {self.log_file}")
            except Exception as e:
                self.get_logger().warning(f"No se pudo eliminar log anterior: {e}")

        # ================= SENSORES ULTRASONICOS =================
        self.ultrasonic_data = {
            'MIT': 0, 'MID': 0, 'MFI': 0, 
            'MFD': 0, 'MDD': 0, 'MDT': 0,
            'LIT': 0, 'LID': 0, 'LFI': 0,
            'LFD': 0, 'LDD': 0, 'LDT': 0
        }

        # ================= ESTADO =====================
        self.last_cmd = None
        self.last_state = ""
        self.arduino_busy = False
        self.timeout_timer = None
        self.ultrasonic_data = {}

        # ================= SERIAL =====================
        try:
            self.ser = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)
            time.sleep(2)
            self.ser.reset_input_buffer()
            self.get_logger().info("Serial conectado al Arduino")
            
            self.serial_thread = threading.Thread(target=self.listen_arduino, daemon=True)
            self.serial_thread.start()
            
        except Exception as e:
            self.get_logger().error(f"Error serial: {e}")
            self.ser = None

#HOLA AMIGUITOS SOY PAOLA YEEEEEIIIII

        # ================= ROS ========================
        self.scan_subscription = self.create_subscription(
            LaserScan,
            'scan',
            self.scan_callback,
            10
        )
        self.cmd_publisher = self.create_publisher(Twist, 'cmd_vel', 10)

    def obstacle_far(self):
        front_ultra = min(
            self.get_ultrasonic_value('MFI'),
            self.get_ultrasonic_value('MFD')
        )
        return front_ultra > 0 and front_ultra < 150

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
            mask = (angles_deg >= angle_start_deg) & (angles_deg <= angle_end_deg)
        else:
            # Cruza 0°
            mask = (angles_deg >= angle_start_deg) | (angles_deg <= angle_end_deg)

        return ranges[mask]
    
    # ================= ESCUCHAR ARDUINO =================
    def listen_arduino(self):
        while True:
            if self.ser and self.ser.in_waiting > 0:
                try:
                    line = self.ser.readline().decode('utf-8').strip()
                    
                    # Detectar fin de secuencia
                    if line == 'T':
                        self.arduino_busy = False
                        self.last_state = ""
                        self.last_cmd = None
                        if self.timeout_timer:
                            self.timeout_timer.cancel()
                            self.timeout_timer = None
                        self.get_logger().info(" Arduino termino secuencia")
                    
                    # Parsear datos de sensores (JSON)
                    elif line.startswith('{') and line.endswith('}'):
                        try:
                            sensor_data = json.loads(line)
                            self.ultrasonic_data.update(sensor_data)
                            self.print_sensor_data(sensor_data)
                        except json.JSONDecodeError:
                            self.get_logger().warning(f"JSON invalido: {line}")
                    
                    # Filtrar mensajes de debug del control remoto
                    elif line.startswith("Ch #") or line == "------------":
                        pass
                    
                    elif line:
                        self.get_logger().info(f"🔊 Arduino: {line}")
                        
                except Exception as e:
                    self.get_logger().warning(f"Error leyendo Arduino: {e}")
            time.sleep(0.01)



    # ================= IMPRIMIR SENSORES =================
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

    # ================= OBTENER VALOR SENSOR =================
    def get_ultrasonic_value(self, sensor_name):
        return self.ultrasonic_data.get(sensor_name, 0)

    # ================= TIMEOUT DE SEGURIDAD =================
    def reset_busy(self):
        if self.arduino_busy:
            self.get_logger().warning("⏰ Timeout - Desbloqueando Arduino forzosamente")
            self.arduino_busy = False
            self.last_state = ""
            self.timeout_timer = None

    # ================= ENVIO SERIAL =================
    def send_command(self, cmd):
        if self.arduino_busy:
            self.get_logger().warning(f"⏸️ Arduino ocupado - Ignorando: {cmd}")
            return
            
        if cmd == self.last_cmd and cmd == 'F':
            return
            
        self.last_cmd = cmd
        self.log_direction(cmd, source="send_command")
        
        if self.ser:
            if cmd in ['L', 'R']:
                self.arduino_busy = True
                # self.timeout_timer = threading.Timer(10.0, self.reset_busy)
                # self.timeout_timer.start()
                self.get_logger().info(f"🔒 Arduino ocupado - Giro {cmd}")
            
            self.ser.write((cmd + '\n').encode())
        else:
            self.get_logger().warning(f"⚠️ Arduino NO conectado: {cmd}")
            
        self.get_logger().info(f"📤 Arduino <- {cmd}")

    # ================= REGISTRO JSON =================
    def log_direction(self, cmd, source="send_command"):
        if cmd not in ("F", "L", "R", "S"):
            return

        entry = {
            "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "command": cmd,
            "source": source
        }

        with self._log_lock:
            try:
                data = []

                if os.path.exists(self.log_file):
                    with open(self.log_file, "r", encoding="utf-8") as f:
                        try:
                            data = json.load(f)
                            if not isinstance(data, list):
                                data = []
                        except json.JSONDecodeError:
                            data = []

                data.append(entry)

                tmp_path = self.log_file + ".tmp"
                with open(tmp_path, "w", encoding="utf-8") as f:
                    json.dump(data, f, ensure_ascii=False, indent=2)

                os.replace(tmp_path, self.log_file)

            except Exception as e:
                self.get_logger().warning(f"⚠️ Error log JSON: {e}")

    # ================= LIDAR CALLBACK ================
    def scan_callback(self, msg):
        if self.arduino_busy:
            return
            
        ranges = np.array(msg.ranges)
        ranges[ranges == 0] = self.max_distance_fr
        ranges[np.isnan(ranges)] = self.max_distance_fr
        ranges[np.isinf(ranges)] = self.max_distance_fr
        
        n = len(ranges)
        front = self.get_sector(msg, 350, 10)
        left  = self.get_sector(msg, 85, 95)
        right = self.get_sector(msg, 265, 275)

        def avg_min(sector):
            return np.mean(np.sort(sector)[:5])

        dist_front = avg_min(front)
        dist_left  = avg_min(left)
        dist_right = avg_min(right)
        
        # ---------- ULTRASONIDOS LEJANOS ----------
        if self.obstacle_far():
            state = "🟡 Obstáculo LEJANO detectado (ultrasonido)"
            if state != self.last_state:
                self.send_command('S')  # o velocidad baja
                self.get_logger().info(state)
                self.last_state = state
            return

        if dist_front < self.min_distance_fr:
            if dist_left > dist_right:
                state = f"🔄 Obst. FRONTAL ({dist_front:.2f}m) - Giro IZQ"
                if state != self.last_state:
                    self.send_command('L')
                    self.get_logger().warn(state)
                    self.last_state = state
            else:
                state = f"🔄 Obst. FRONTAL ({dist_front:.2f}m) - Giro DER"
                if state != self.last_state:
                    self.send_command('R')
                    self.get_logger().warn(state)
                    self.last_state = state

        elif dist_left < self.min_distance_ld and dist_front < self.min_distance_fr:
            state = f"⚠️ Obst. IZQ ({dist_left:.2f}m) - Ajuste DER"
            if state != self.last_state:
                self.send_command('R')
                self.get_logger().warn(state)
                self.last_state = state

        elif dist_right < self.min_distance_ld and dist_front < self.min_distance_fr:
            state = f"⚠️ Obst. DER ({dist_right:.2f}m) - Ajuste IZQ"
            if state != self.last_state:
                self.send_command('L')
                self.get_logger().warn(state)
                self.last_state = state

        else:
            state = f"✅ Avanzando (F:{dist_front:.2f} L:{dist_left:.2f} R:{dist_right:.2f})"
            if state != self.last_state:
                self.send_command('F')
                self.get_logger().info(f">>> {state}")
                self.last_state = state

    # ================= CIERRE SEGURO =================
    def destroy_node(self):
        if self.timeout_timer:
            self.timeout_timer.cancel()
        if self.ser:
            self.ser.write(b'S\n')
            self.ser.close()
        super().destroy_node()

def main(args=None):
    rclpy.init(args=args)
    node = ObstacleAvoidance()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.send_command('S')
    finally:
        node.destroy_node()
        rclpy.shutdown()

main()