import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge

from robot_vision.qos import CAMERA_QOS

# Ruta estable de la capturadora "AV TO USB2.0" (chip MacroSilicon). Se usa en
# vez de un indice /dev/videoN porque ese numero cambia si se reordena el USB.
# Otra camara: --ros-args -p camera_device:=/dev/v4l/by-id/<...> (ver ls /dev/v4l/by-id).
DEFAULT_CAMERA_DEVICE = '/dev/v4l/by-id/usb-MACROSILICON_AV_TO_USB2.0_20150130-video-index0'

# Formato de la capturadora (ver `v4l2-ctl -d <dev> --list-formats-ext`). Si la
# camara no lo ofrece, V4L2 elige el mas cercano y el log de arranque lo dice.
# La resolucion se puede cambiar al arrancar:
#   --ros-args -p camera_width:=720 -p camera_height:=480
CAMERA_FOURCC = 'MJPG'
DEFAULT_CAMERA_WIDTH = 480
DEFAULT_CAMERA_HEIGHT = 320
CAMERA_FPS = 25
# Un solo buffer: si el consumidor se atrasa se pierde un frame en vez de
# acumular latencia con frames viejos.
CAMERA_BUFFER_FRAMES = 1

# El timer va mas rapido que la camara: read() bloquea hasta el siguiente
# frame, asi la camara marca el ritmo y no el timer.
FRAME_RATE_HZ = 30.0


class CameraRawPublisher(Node):
    def __init__(self):
        super().__init__('camera_raw_publisher')

        # Se leen solo al arrancar: cambiarlos en caliente no reabre la camara.
        self.declare_parameter('camera_device', DEFAULT_CAMERA_DEVICE)
        self.declare_parameter('camera_width', DEFAULT_CAMERA_WIDTH)
        self.declare_parameter('camera_height', DEFAULT_CAMERA_HEIGHT)
        self.cap = self._open_camera(
            self.get_parameter('camera_device').value,
            self.get_parameter('camera_width').value,
            self.get_parameter('camera_height').value,
        )

        self.bridge = CvBridge()
        self.publisher_ = self.create_publisher(Image, '/camera/image_raw', CAMERA_QOS)
        self.timer = self.create_timer(1.0 / FRAME_RATE_HZ, self.publish_frame)
        self.get_logger().info('camera_raw_publisher iniciado')

    def _open_camera(self, device, width, height):
        # CAP_V4L2 explicito: sin el, OpenCV puede caer al backend FFMPEG, donde
        # el formato, los fps y el buffer no se pueden fijar.
        cap = cv2.VideoCapture(device, cv2.CAP_V4L2)
        if not cap.isOpened():
            self.get_logger().error(f'no se pudo abrir la camara ({device})')
            return cap

        # En V4L2 el orden importa: primero el formato, luego resolucion y fps.
        cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*CAMERA_FOURCC))
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
        cap.set(cv2.CAP_PROP_FPS, CAMERA_FPS)
        cap.set(cv2.CAP_PROP_BUFFERSIZE, CAMERA_BUFFER_FRAMES)

        # Lo que la camara realmente acepto, no lo que se pidio.
        fourcc = int(cap.get(cv2.CAP_PROP_FOURCC)).to_bytes(4, 'little').decode(errors='replace')
        actual_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        actual_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        fps = cap.get(cv2.CAP_PROP_FPS)
        self.get_logger().info(
            f'camara abierta: {device} -> pedido {width}x{height}, '
            f'aceptado {actual_width}x{actual_height} {fourcc} @ {fps:.0f} fps')
        return cap

    def publish_frame(self):
        ret, frame = self.cap.read()
        if not ret:
            self.get_logger().warn('no se pudo leer un frame')
            return

        msg = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')
        msg.header.stamp = self.get_clock().now().to_msg()
        self.publisher_.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = CameraRawPublisher()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
