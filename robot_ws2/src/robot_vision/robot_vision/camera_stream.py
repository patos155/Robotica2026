"""
Nodo dueno unico de la camara fisica. Publica frames crudos a un
topic ROS2 (sensor_msgs/Image) via cv_bridge - no dibuja nada, no
decide nada, solo captura y publica. Cualquier nodo que necesite ver
la camara (inspection_pipeline.py, y a futuro navigation.py) se
suscribe a este topic en vez de abrir el dispositivo por su cuenta.
"""
import cv2
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from sensor_msgs.msg import Image
from cv_bridge import CvBridge


class CameraStream(Node):

    def __init__(self):
        super().__init__('camera_stream')

        # Parametros en vez de hardcodear - equivalente a params.yaml,
        # mismo principio que config.h del lado de Arduino.
        self.declare_parameter('device_index', 0)
        self.declare_parameter('width', 1920)
        self.declare_parameter('height', 1080)
        self.declare_parameter('fps', 15.0)

        device_index = self.get_parameter('device_index').value
        width = self.get_parameter('width').value
        height = self.get_parameter('height').value
        fps = self.get_parameter('fps').value

        self.cap = cv2.VideoCapture(device_index)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

        if not self.cap.isOpened():
            self.get_logger().error(f'No se pudo abrir la camara (index={device_index})')

        self.bridge = CvBridge()

        # Este topic vive DENTRO de la laptop, nunca sale por WiFi -
        # no compite con el limite de 5 Mbps del reglamento. Reliable
        # esta bien aqui; el cuidado de ancho de banda aplica recien
        # en el stream que sale hacia el dashboard via web_video_server.
        qos = QoSProfile(
            reliability=ReliabilityPolicy.RELIABLE,
            history=HistoryPolicy.KEEP_LAST,
            depth=1,
        )
        self.publisher_ = self.create_publisher(Image, '/camera/image_raw', qos)
        self.timer = self.create_timer(1.0 / fps, self.publish_frame)
        self.get_logger().info(f'camera_stream iniciado - {width}x{height} @ {fps}fps')

    def publish_frame(self):
        ret, frame = self.cap.read()
        if not ret:
            self.get_logger().warn('Frame no disponible')
            return
        msg = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')
        msg.header.stamp = self.get_clock().now().to_msg()
        self.publisher_.publish(msg)

    def destroy_node(self):
        self.cap.release()
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = CameraStream()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
