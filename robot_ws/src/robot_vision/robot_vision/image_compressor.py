import time

import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image, CompressedImage
from cv_bridge import CvBridge

from robot_vision.qos import CAMERA_QOS

# Parametros ROS que definen el bitrate del video hacia el dashboard. Se
# cambian en vivo, sin reiniciar el nodo:
#   ros2 param set /image_compressor max_fps 5.0
# max_fps y scale son double: usa "5.0", no "5" (con un entero ROS2 rechaza el
# cambio). Los valores fuera de rango se acotan a los limites de abajo.
#
# Perfiles de referencia con la capturadora actual (480x320):
#   normal: max_fps 12.0, scale 1.0,  jpeg_quality 60  (~2.2 Mbps)
#   ahorro: max_fps 5.0,  scale 0.67, jpeg_quality 40  (~0.5 Mbps, estimado)
DEFAULT_MAX_FPS = 12.0
DEFAULT_SCALE = 1.0
DEFAULT_JPEG_QUALITY = 60
MAX_FPS_RANGE = (1.0, 30.0)
SCALE_RANGE = (0.1, 1.0)
JPEG_QUALITY_RANGE = (1, 100)
NS_PER_S = 1_000_000_000


def _clamp(value, bounds):
    return min(max(value, bounds[0]), bounds[1])


def throttle(now_ns, next_ns, interval_ns):
    """Decide si publicar el frame que llego en `now_ns`.

    Regresa (publicar, next_ns nuevo). Agenda por reloj en vez de medir el
    tiempo desde el ultimo frame publicado: asi max_fps se cumple en promedio
    aunque no divida los fps de la camara (12 fps con una camara a 25 fps).
    Tras una pausa larga deja pasar como mucho un frame extra, no una rafaga.
    """
    if now_ns < next_ns:
        return False, next_ns
    return True, max(next_ns, now_ns - interval_ns) + interval_ns


class ImageCompressor(Node):
    def __init__(self):
        super().__init__('image_compressor')
        self.bridge = CvBridge()
        self.next_publish_ns = 0

        self.declare_parameter('max_fps', DEFAULT_MAX_FPS)
        self.declare_parameter('scale', DEFAULT_SCALE)
        self.declare_parameter('jpeg_quality', DEFAULT_JPEG_QUALITY)

        self.create_subscription(Image, '/camera/image_raw', self.on_frame, CAMERA_QOS)
        self.publisher_ = self.create_publisher(CompressedImage, '/camera/image_raw/compressed', CAMERA_QOS)
        self.get_logger().info('image_compressor iniciado')

    def on_frame(self, msg):
        # Los parametros se leen en cada frame para que `ros2 param set` surta
        # efecto de inmediato. El limite de fps va antes de convertir el frame:
        # los que se descartan no cuestan conversion ni codificacion.
        max_fps = _clamp(self.get_parameter('max_fps').value, MAX_FPS_RANGE)
        publish, self.next_publish_ns = throttle(
            time.monotonic_ns(), self.next_publish_ns, int(NS_PER_S / max_fps))
        if not publish:
            return

        scale = _clamp(self.get_parameter('scale').value, SCALE_RANGE)
        quality = _clamp(self.get_parameter('jpeg_quality').value, JPEG_QUALITY_RANGE)

        frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        if scale < 1.0:
            frame = cv2.resize(frame, None, fx=scale, fy=scale, interpolation=cv2.INTER_AREA)

        encode_param = [cv2.IMWRITE_JPEG_QUALITY, quality]
        success, encoded = cv2.imencode('.jpg', frame, encode_param)
        if not success:
            self.get_logger().warn('no se pudo comprimir el frame')
            return

        out = CompressedImage()
        out.header = msg.header
        out.format = 'jpeg'
        out.data = encoded.tobytes()
        self.publisher_.publish(out)


def main(args=None):
    rclpy.init(args=args)
    node = ImageCompressor()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
