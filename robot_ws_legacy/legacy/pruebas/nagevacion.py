import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Twist
import numpy as np
import serial
import time

# ================= SECUENCIAS DE GIRO =================
giroIzq = ["L"] * 12
giroDer = ["R"] * 12

class ObstacleAvoidance(Node):
    def __init__(self):
        super().__init__('obstacle_avoidance')

        # ================= PARÁMETROS =================
        self.declare_parameter('min_distance_fr', 1.0)
        self.declare_parameter('max_distance_fr', 2.0)
        self.declare_parameter('min_distance_ld', 0.5)
        self.declare_parameter('max_distance_ld', 0.8)

        self.min_distance_fr = self.get_parameter('min_distance_fr').value
        self.max_distance_fr = self.get_parameter('max_distance_fr').value
        self.min_distance_ld = self.get_parameter('min_distance_ld').value
        self.max_distance_ld = self.get_parameter('max_distance_ld').value

        # ================= ESTADO =====================
        self.last_cmd = None
        self.last_state = ""
        self.turning = False
        self.turn_sequence = []
        self.turn_index = 0

        # ================= SERIAL =====================
        try:
            self.ser = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)
            time.sleep(2)
            self.ser.reset_input_buffer()
            self.get_logger().info("Serial conectado al Arduino")
        except Exception as e:
            self.get_logger().error(f"Error serial: {e}")
            self.ser = None

        # ================= ROS ========================
        self.scan_subscription = self.create_subscription(
            LaserScan,
            'scan',
            self.scan_callback,
            10
        )
        self.cmd_publisher = self.create_publisher(Twist, 'cmd_vel', 10)

    # ================= ENVÍO SERIAL =================
    def send_command(self, cmd):
        if cmd == self.last_cmd:
            return
        self.last_cmd = cmd
        if self.ser:
            self.ser.write((cmd + '\n').encode())
            self.get_logger().info(f"📤 Arduino <- {cmd}")
        else:
            self.get_logger().warning(f"⚠️ Arduino NO conectado - Comando ignorado: {cmd}")

    # ================= INICIAR GIRO =================
    def start_turn(self, direction):
        if self.turning:
            return
        self.turn_sequence = giroIzq if direction == 'L' else giroDer
        self.turn_index = 0
        self.turning = True
        direction_name = "IZQUIERDA" if direction == 'L' else "DERECHA"
        self.get_logger().info(f"🔄 Iniciando giro {direction_name}")

    # ================= LIDAR CALLBACK ================
    def scan_callback(self, msg):

        # ======== EJECUTANDO GIRO =========
        if self.turning:
            if self.turn_index < len(self.turn_sequence):
                cmd = self.turn_sequence[self.turn_index]
                self.send_command(cmd)
                self.turn_index += 1
                return
            else:
                self.turning = False
                self.turn_sequence = []
                self.turn_index = 0
                self.get_logger().info(f"✅ Giro completado")
                return

        # ================= PROCESAR LIDAR =================
        ranges = np.array(msg.ranges)
        ranges[ranges == 0] = self.max_distance_fr
        ranges[np.isnan(ranges)] = self.max_distance_fr
        ranges[np.isinf(ranges)] = self.max_distance_fr
        

        n = len(ranges)
        front = np.concatenate([ranges[-n*30//360:], ranges[:n*30//360]])
        left  = ranges[n*60//360:n*120//360]
        right = ranges[n*240//360:n*300//360]

        def avg_min(sector):
            return np.mean(np.sort(sector)[:5])

        dist_front = avg_min(front)
        dist_left  = avg_min(left)
        dist_right = avg_min(right)


        # ================= DECISION ===================
        if dist_front < self.min_distance_fr:
            # Obstáculo frontal detectado
            if dist_left > dist_right:
                self.start_turn('L')
            else:
                self.start_turn('R')
        elif dist_left < self.min_distance_ld:
            # Obstáculo muy cerca por la izquierda → girar derecha
            state = f"⚠️ Obst. IZQ ({dist_left:.2f}m) - Ajustando DERECHA"
            if state != self.last_state:
                self.start_turn('R')
                self.get_logger().warn(state)
                self.last_state = state
        elif dist_right < self.min_distance_ld:
            # Obstáculo muy cerca por la derecha → girar izquierda
            state = f"⚠️ Obst. DER ({dist_right:.2f}m) - Ajustando IZQUIERDA"
            if state != self.last_state:
                self.start_turn('L')
                self.get_logger().warn(state)
                self.last_state = state
        else:
            # Camino libre
            state = f"Avanzando (F:{dist_front:.2f} L:{dist_left:.2f} R:{dist_right:.2f})"
            if state != self.last_state:
                self.send_command('F')
                self.get_logger().info(f">>> {state}")
                self.last_state = state

    # ================= CIERRE SEGURO =================
    def destroy_node(self):
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