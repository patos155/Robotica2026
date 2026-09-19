import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image, CompressedImage
from cv_bridge import CvBridge

from robot_vision.qos import CAMERA_QOS

JPEG_QUALITY = 60


class ImageCompressor(Node):
    def __init__(self):
        super().__init__('image_compressor')
        self.bridge = CvBridge()

        self.create_subscription(Image, '/camera/image_raw', self.on_frame, CAMERA_QOS)
        self.publisher_ = self.create_publisher(CompressedImage, '/camera/image_raw/compressed', CAMERA_QOS)
        self.get_logger().info('image_compressor iniciado')

    def on_frame(self, msg):
        frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')

        encode_param = [cv2.IMWRITE_JPEG_QUALITY, JPEG_QUALITY]
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
