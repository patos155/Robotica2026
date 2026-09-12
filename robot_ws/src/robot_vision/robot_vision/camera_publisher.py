import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import CompressedImage
from cv_bridge import CvBridge
import os
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy


class CameraPublisher(Node):
    def __init__(self):
        super().__init__('camera_publisher')

        self.cap = cv2.VideoCapture(1)
        # os.environ['OPENCV_FFMPEG_CAPTURE_OPTIONS'] = 'rtsp_transport;tcp|fflags;nobuffer|flags;low_delay'
        # self.cap = cv2.VideoCapture('rtsp://host.docker.internal:8554/cam')
        if not self.cap.isOpened():
            self.get_logger().error('No se pudo abrir la camara (index 0)')

        self.bridge = CvBridge()

        qos = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1
        )

        self.publisher_ = self.create_publisher(CompressedImage, '/camera/image_raw', 10)
        self.timer = self.create_timer(1.0 / 30.0, self.publish_frame)
        self.get_logger().info('camera_publisher iniciado')

    def publish_frame(self):
        ret, frame = self.cap.read()
        if not ret:
            self.get_logger().warn('no se pudo leer un frame')
            return

        encode_param = [cv2.IMWRITE_JPEG_QUALITY, 60]
        success, encoded = cv2.imencode('.jpg', frame, encode_param)
        if not success:
            self.get_logger().warn('No se pudo codificar el frame')
            return
        
        msg = CompressedImage()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.format = 'jpeg'
        msg.data = encoded.tobytes()
        # msg = self.bridge.cv2_to_compressed_imgmsg(frame, dst_format='jpg')
        self.publisher_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = CameraPublisher()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
