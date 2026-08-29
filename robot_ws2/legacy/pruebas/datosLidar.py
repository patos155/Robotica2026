import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Twist
import numpy as np

class ObstacleAvoidance(Node):
    def __init__(self):
        super().__init__('obstacle_avoidance')
        
        # ============================================================
        # Parámetros configurables
        # ============================================================
        self.declare_parameter('min_distance', 0.5)  # Distancia mínima al frente
        self.declare_parameter('wall_distance', 0.15)  # Distancia mínima a paredes laterales
        self.declare_parameter('max_distance', 2.0)
        self.declare_parameter('speed', 0.5)
        self.declare_parameter('angular_speed', 0.8)

        # Cargar parámetros
        self.min_distance = self.get_parameter('min_distance').value
        self.wall_distance = self.get_parameter('wall_distance').value
        self.max_distance = self.get_parameter('max_distance').value
        self.speed = self.get_parameter('speed').value
        self.angular_speed = self.get_parameter('angular_speed').value

        # Último estado
        self.last_state = "INIT"

        # ============================================================
        # Subscripción al LIDAR
        # ============================================================
        self.scan_subscription = self.create_subscription(
            LaserScan,
            'scan',
            self.scan_callback,
            10)
        
        # Publicador de velocidades
        self.cmd_publisher = self.create_publisher(Twist, 'cmd_vel', 10)
        
        self.get_logger().info('=' * 60)
        self.get_logger().info('Sistema de navegación en pistas iniciado')
        self.get_logger().info(f'Distancia mínima FRENTE: {self.min_distance}m')
        self.get_logger().info(f'Distancia mínima PAREDES: {self.wall_distance}m')
        self.get_logger().info('=' * 60)
        #a = input()

    # ============================================================
    # FUNCION SIMULADA PARA COMANDOS
    # ============================================================
    def send_command(self, cmd):
        # Solo loguea el comando, no envía nada por Arduino
        self.get_logger().info(f"📤 Comando simulado: {cmd}")
        #b = input()

    # ============================================================
    # PROCESAMIENTO DEL LIDAR
    # ============================================================
    def scan_callback(self, msg):
        ranges = np.array(msg.ranges)
        ranges[ranges == 0] = float('inf')
        ranges[np.isinf(ranges)] = self.max_distance
        ranges[np.isnan(ranges)] = self.max_distance
        
        num_points = len(ranges)
        
        # ===================== SECTORES ============================
        front_angle = int(num_points * 30 / 360)
        front_sector = np.concatenate([ranges[-front_angle:], ranges[:front_angle]])
        left_sector = ranges[int(num_points * 60 / 360):int(num_points * 120 / 360)]
        right_sector = ranges[int(num_points * 240 / 360):int(num_points * 300 / 360)]
        
        # ===================== DISTANCIAS ==========================
        def avg_min(sector, n=5):
            if len(sector) == 0:
                return self.max_distance
            sorted_sector = np.sort(sector)
            return np.mean(sorted_sector[:min(n, len(sorted_sector))])
        
        dist_front = avg_min(front_sector)
        dist_left = avg_min(left_sector)
        dist_right = avg_min(right_sector)
        
        # ===================== DECISIONES ==========================
        cmd = Twist()
        current_state = ""
        margin = 0.05
        
        if dist_front < self.min_distance:
            cmd.linear.x = 0.0
            if dist_left < self.wall_distance*2 and dist_right < self.wall_distance*2:
                # Ambas paredes a los lados, pero no tan cerradas: avanzar despacio
                cmd.linear.x = self.speed * 0.5
                cmd.angular.z = 0.0
                current_state = "PASILLO - Avanzando despacio entre paredes"
                self.send_command('F')
            elif dist_left > dist_right + margin:
                cmd.angular.z = self.angular_speed
                current_state = f"GIRANDO IZQ - Obstáculo frente ({dist_front:.2f}m)"
                self.send_command('L')
            else:
                cmd.angular.z = -self.angular_speed
                current_state = f"GIRANDO DER - Obstáculo frente ({dist_front:.2f}m)"
                self.send_command('R')
        
        elif dist_left < self.wall_distance and dist_front > self.min_distance:
            cmd.linear.x = self.speed * 0.7
            cmd.angular.z = -self.angular_speed * 0.2
            current_state = f"Cerca pared IZQ ({dist_left:.2f}m) - Ajustando"
            self.send_command('R')
        
        elif dist_right < self.wall_distance and dist_front > self.min_distance:
            cmd.linear.x = self.speed * 0.7
            cmd.angular.z = self.angular_speed * 0.2
            current_state = f"Cerca pared DER ({dist_right:.2f}m) - Ajustando"
            self.send_command('L')
        
        elif dist_front > self.min_distance:
            cmd.linear.x = self.speed
            cmd.angular.z = 0.0
            current_state = f"Avanzando recto (L:{dist_left:.2f} R:{dist_right:.2f})"
            self.send_command('F')
        
        else:
            cmd.linear.x = 0.0
            cmd.angular.z = 0.0
            current_state = "DETENIDO - Precaución"
            self.send_command('S')

        # ===================== LOGGING ============================
        if current_state != self.last_state:
            self.get_logger().info(f'>>> {current_state}')
            self.last_state = current_state
        
        # Publicar comando en ROS
        self.cmd_publisher.publish(cmd)

def main(args=None):
    rclpy.init(args=args)
    node = ObstacleAvoidance()
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.send_command('S')
        node.get_logger().info('🟥 Sistema detenido manualmente')
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
