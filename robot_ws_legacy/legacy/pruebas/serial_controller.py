import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import serial
import time

class ArduinoController(Node):
    def __init__(self):
        super().__init__('arduino_controller')
        
        # Declarar parámetro del puerto serial
        self.declare_parameter('serial_port', '/dev/ttyUSB1')
        self.declare_parameter('baud_rate', 115200)
        
        port = self.get_parameter('serial_port').value
        baud = self.get_parameter('baud_rate').value
        
        try:
            # Conectar al Arduino
            self.serial_port = serial.Serial(port, baud, timeout=1)
            time.sleep(2)  # Esperar que Arduino se reinicie
            self.get_logger().info(f' Conectado a Arduino en {port}')
        except Exception as e:
            self.get_logger().error(f' Error al conectar: {e}')
            return
        
        # Suscribirse a comandos de velocidad
        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.cmd_vel_callback,
            10)
        
        self.get_logger().info('Esperando comandos en /cmd_vel...')
    
    def cmd_vel_callback(self, msg):
        linear = msg.linear.x
        angular = msg.angular.z

        # 🔁 PRIORIDAD: primero giros, luego avance
        if abs(angular) > 0.1:
            if angular > 0:
                self.send_command('L')  # Izquierda
            else:
                self.send_command('R')  # Derecha
        elif abs(linear) > 0.1:
            if linear > 0:
                self.send_command('F')  # Adelante
        else:
            self.send_command('S')  # Stop

    
    def send_command(self, cmd):
        try:
            self.serial_port.write(cmd.encode())
            # Leer respuesta del Arduino (opcional)
            response = self.serial_port.readline().decode().strip()
            if response:
                self.get_logger().info(f'Arduino: {response}')
        except Exception as e:
            self.get_logger().error(f'Error enviando comando: {e}')

def main(args=None):
    rclpy.init(args=args)
    controller = ArduinoController()
    
    try:
        rclpy.spin(controller)
    except KeyboardInterrupt:
        pass
    
    controller.destroy_node()
    rclpy.shutdown()

main()