import time

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
from robot_vision.detectors.motion_detector import create_subtractor, detect_motion

JPEG_QUALITY = 60
EXECUTOR_THREADS = 4  # suscripcion de frames + goal/cancel/execute concurrentes
DEFAULT_DURATION_TIMEOUT_S = 30  # usado cuando el goal manda timeout_s = 0
MOTION_WARMUP_FRAMES = 15  # frames que se descartan del resultado mientras el subtractor (nuevo en cada prueba) aprende el fondo
MOTION_FEEDBACK_INTERVAL_S = 0.2


def _encode_jpeg(frame):
    success, encoded = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, JPEG_QUALITY])
    if not success:
        return None
    msg = CompressedImage()
    msg.format = 'jpeg'
    msg.data = encoded.tobytes()
    return msg


def _handle_qr(node, goal_handle):
    result = RunTest.Result()
    frame = node.latest_frame
    if frame is None:
        goal_handle.abort()
        result.message = 'todavia no ha llegado ningun frame de camera_raw_publisher'
        return result

    annotated, qr_text = detect_qr(frame)
    img_msg = _encode_jpeg(annotated)
    if img_msg is None:
        goal_handle.abort()
        result.message = 'no se pudo codificar la imagen anotada'
        return result

    goal_handle.succeed()
    result.success = True
    result.message = 'QR detectado' if qr_text else 'sin QR en el frame'
    result.result_text = qr_text
    result.result_images = [img_msg]
    return result


def _handle_motion(node, goal_handle):
    # Prueba de duracion: corre hasta que la cancelen o se agote el timeout,
    # mandando feedback periodico con el frame anotado. El subtractor se crea
    # nuevo en cada prueba (mas simple, sin costo en reposo) a cambio de que
    # los primeros MOTION_WARMUP_FRAMES no cuenten para el resultado mientras
    # aprende el fondo -- si no, casi toda prueba arrancaria con un falso
    # positivo.
    timeout_s = goal_handle.request.timeout_s or DEFAULT_DURATION_TIMEOUT_S
    subtractor = create_subtractor()

    result = RunTest.Result()
    start = node.get_clock().now()
    frames_seen = 0
    motion_ever_detected = False
    last_annotated = None

    while True:
        elapsed_s = (node.get_clock().now() - start).nanoseconds / 1e9
        if elapsed_s >= timeout_s:
            result.message = 'tiempo agotado'
            break

        if goal_handle.is_cancel_requested:
            goal_handle.canceled()
            result.message = 'prueba cancelada'
            return result

        frame = node.latest_frame
        if frame is not None:
            annotated, motion_found = detect_motion(frame, subtractor)
            frames_seen += 1
            last_annotated = annotated

            if frames_seen > MOTION_WARMUP_FRAMES and motion_found:
                motion_ever_detected = True

            feedback_img = _encode_jpeg(annotated)
            if feedback_img is not None:
                feedback = RunTest.Feedback()
                feedback.status = 'movimiento detectado' if motion_found else 'sin movimiento'
                feedback.preview_image = feedback_img
                goal_handle.publish_feedback(feedback)

        time.sleep(MOTION_FEEDBACK_INTERVAL_S)

    goal_handle.succeed()
    result.success = True
    result.result_text = 'movimiento detectado' if motion_ever_detected else 'sin movimiento'
    if last_annotated is not None:
        img_msg = _encode_jpeg(last_annotated)
        if img_msg is not None:
            result.result_images = [img_msg]
    return result


# Registro de pruebas: agregar hazmat/voz aqui con el mismo patron
# (test_id -> funcion(node, goal_handle) -> RunTest.Result).
TEST_HANDLERS = {
    'qr': _handle_qr,
    'motion': _handle_motion,
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
