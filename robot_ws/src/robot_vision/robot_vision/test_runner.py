import cv2
import rclpy
from rclpy.action import ActionServer, CancelResponse
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from sensor_msgs.msg import Image, CompressedImage
from cv_bridge import CvBridge

from robot_interfaces.action import RunTest
from robot_vision.qos import CAMERA_QOS
from robot_vision.detectors.qr_detector import detect_qr

JPEG_QUALITY = 60
EXECUTOR_THREADS = 4  # suscripcion de frames + goal/cancel/execute concurrentes


def _handle_qr(node, goal_handle):
    result = RunTest.Result()
    frame = node.latest_frame
    if frame is None:
        goal_handle.abort()
        result.message = 'todavia no ha llegado ningun frame de camera_raw_publisher'
        return result

    annotated, qr_text = detect_qr(frame)
    success, encoded = cv2.imencode('.jpg', annotated, [cv2.IMWRITE_JPEG_QUALITY, JPEG_QUALITY])
    if not success:
        goal_handle.abort()
        result.message = 'no se pudo codificar la imagen anotada'
        return result

    img_msg = CompressedImage()
    img_msg.format = 'jpeg'
    img_msg.data = encoded.tobytes()

    goal_handle.succeed()
    result.success = True
    result.message = 'QR detectado' if qr_text else 'sin QR en el frame'
    result.result_text = qr_text
    result.result_images = [img_msg]
    return result


# Registro de pruebas: agregar hazmat/movimiento/voz aqui con el mismo
# patron (test_id -> funcion(node, goal_handle) -> RunTest.Result).
TEST_HANDLERS = {
    'qr': _handle_qr,
}


class TestRunner(Node):
    def __init__(self):
        super().__init__('test_runner')
        self.bridge = CvBridge()
        self.latest_frame = None

        self.create_subscription(Image, '/camera/image_raw', self.on_frame, CAMERA_QOS)

        self._action_server = ActionServer(
            self, RunTest, 'run_test',
            execute_callback=self.execute,
            cancel_callback=self.cancel,
            callback_group=ReentrantCallbackGroup(),
        )
        self.get_logger().info('test_runner iniciado')

    def on_frame(self, msg):
        self.latest_frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')

    def cancel(self, goal_handle):
        return CancelResponse.ACCEPT

    def execute(self, goal_handle):
        handler = TEST_HANDLERS.get(goal_handle.request.test_id)
        if handler is None:
            goal_handle.abort()
            result = RunTest.Result()
            result.message = f'test_id desconocido: {goal_handle.request.test_id}'
            return result
        return handler(self, goal_handle)


def main(args=None):
    rclpy.init(args=args)
    node = TestRunner()
    executor = MultiThreadedExecutor(num_threads=EXECUTOR_THREADS)
    try:
        rclpy.spin(node, executor)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
