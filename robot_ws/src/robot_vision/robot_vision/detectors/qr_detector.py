import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
from pyzbar.pyzbar import decode
import numpy as np

class QrTest(Node):

    def __init__(self):
        super().__init__('qr_test')
        self.bridge = CvBridge()

        self.create_subscription(Image, '/camera/image_raw', self.on_frame, 10)
        self.processed_pub = self.create_publisher(Image, '/qr_test/image_processed', 10)
        self.get_logger().info('qr_test iniciado s')

    def on_frame(self, msg):
        frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        detected_codes = decode(frame)
        # self.get_logger().info(f'QR Code pending:')
        for code in detected_codes:
            qr_data = code.data.decode('utf-8')
            self.get_logger().info(f'QR Code detected: {qr_data}')

            if len(code.polygon) == 4:
                polygon_points = [(p.x, p.y) for p in code.polygon]
                pts = np.array(polygon_points, np.int32).reshape((-1,1,2))
                cv2.polylines(frame, [pts], True, (0,255,0), 3)

                text_position = polygon_points[0]
                cv2.putText(frame, qr_data, text_position, cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,255), 2)
                # cv2.putText(frame, qr_data, text_position, cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255,0,0), 2)

            processed_msg = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')
            self.processed_pub.publish(processed_msg)


def main(args=None):
    rclpy.init(args=args)
    node = QrTest()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()