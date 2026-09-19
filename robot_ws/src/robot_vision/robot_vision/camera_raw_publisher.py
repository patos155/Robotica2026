import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge

from robot_vision.qos import CAMERA_QOS

CAMERA_INDEX = 1
FRAME_RATE_HZ = 30.0


class CameraRawPublisher(Node):
    def __init__(self):
        super().__init__('camera_raw_publisher')

        self.cap = cv2.VideoCapture(CAMERA_INDEX)
        if not self.cap.isOpened():
            self.get_logger().error(f'no se pudo abrir la camara (index {CAMERA_INDEX})')

        self.bridge = CvBridge()
        self.publisher_ = self.create_publisher(Image, '/camera/image_raw', CAMERA_QOS)
        self.timer = self.create_timer(1.0 / FRAME_RATE_HZ, self.publish_frame)
        self.get_logger().info('camera_raw_publisher iniciado')

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
