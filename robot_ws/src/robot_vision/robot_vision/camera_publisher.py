import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import os


class CameraPublisher(Node):
    def __init__(self):
        super().__init__('camera_publisher')

        # self.cap = cv2.VideoCapture(0)
        os.environ['OPENCV_FFMPEG_CAPTURE_OPTIONS'] = 'rtsp_transport;tcp|fflags;nobuffer|flags;low_delay'
        self.cap = cv2.VideoCapture('rtsp://host.docker.internal:8554/cam')
        if not self.cap.isOpened():
            self.get_logger().error('No se pudo abrir la camara (index 0)')

        self.bridge = CvBridge()
        self.publisher_ = self.create_publisher(Image, '/camera/image_raw', 10)
        self.timer = self.create_timer(1.0 / 30.0, self.publish_frame)
        self.get_logger().info('camera_publisher iniciado')

    def publish_frame(self):
        ret, frame = self.cap.read()
        if not ret:
            self.get_logger().warn('no se pudo leer un frame')
            return
        
        msg = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')
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
