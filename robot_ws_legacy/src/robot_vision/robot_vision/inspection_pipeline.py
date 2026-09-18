"""
Action Server de ExecuteTest. Dueno unico de "que se dibuja" sobre el
video de inspeccion: se suscribe al topic crudo de camera_stream.py
(nunca reabre la camara), corre el detector correspondiente segun la
prueba activa, y publica un segundo topic ya con overlay para que
web_video_server lo exponga como stream de inspeccion aparte del de
navegacion.
"""
import json
import time

import rclpy
from rclpy.action import ActionServer, CancelResponse, GoalResponse
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from sensor_msgs.msg import Image
from cv_bridge import CvBridge

from robot_interfaces.action import ExecuteTest
from robot_vision.detectors import qr_detector


class InspectionPipeline(Node):

    def __init__(self):
        super().__init__('inspection_pipeline')

        self.declare_parameter('qr_timeout_sec', 8.0)
        self.declare_parameter('qr_poll_interval_sec', 0.2)

        self.bridge = CvBridge()
        self.latest_frame = None

        qos = QoSProfile(
            reliability=ReliabilityPolicy.RELIABLE,
            history=HistoryPolicy.KEEP_LAST,
            depth=1,
        )
        self.create_subscription(Image, '/camera/image_raw', self.on_frame, qos)

        # Este SI es el stream que sale hacia el dashboard via
        # web_video_server - candidato a compresion mas adelante si
        # el ancho de banda del WiFi degradado lo exige.
        self.processed_pub = self.create_publisher(Image, '/inspection/image_processed', qos)

        # Reentrante: el nodo debe poder seguir recibiendo frames Y
        # atendiendo cancelaciones de la Action mientras una prueba
        # esta corriendo, no bloquearse en una sola cosa a la vez.
        callback_group = ReentrantCallbackGroup()
        self._action_server = ActionServer(
            self,
            ExecuteTest,
            'execute_test',
            execute_callback=self.execute_callback,
            goal_callback=self.goal_callback,
            cancel_callback=self.cancel_callback,
            callback_group=callback_group,
        )
        self.get_logger().info('inspection_pipeline listo')

    def on_frame(self, msg: Image):
        self.latest_frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        # Aunque no haya prueba activa, se republica el frame tal cual
        # para que el dashboard siempre tenga algo que mostrar en el
        # stream de inspeccion, no solo mientras corre una prueba.
        self.processed_pub.publish(msg)

    def goal_callback(self, goal_request):
        self.get_logger().info(f'Goal recibido: {goal_request.test_name}')
        return GoalResponse.ACCEPT

    def cancel_callback(self, goal_handle):
        self.get_logger().warn('Cancelacion solicitada')
        return CancelResponse.ACCEPT

    def execute_callback(self, goal_handle):
        test_name = goal_handle.request.test_name

        if test_name == 'qr':
            return self._run_qr_test(goal_handle)

        # hazmat, motion, magnet, etc. se agregan aqui con el mismo
        # patron conforme se vayan destrabando (ver CONTEXTO_ROBOT_WS.md)
        goal_handle.abort()
        result = ExecuteTest.Result()
        result.success = False
        result.result_json = json.dumps({'error': f'prueba desconocida: {test_name}'})
        return result

    def _run_qr_test(self, goal_handle):
        timeout = self.get_parameter('qr_timeout_sec').value
        poll_interval = self.get_parameter('qr_poll_interval_sec').value
        start = time.time()

        while time.time() - start < timeout:
            if goal_handle.is_cancel_requested:
                goal_handle.canceled()
                result = ExecuteTest.Result()
                result.success = False
                result.result_json = json.dumps({'reason': 'cancelada'})
                return result

            if self.latest_frame is not None:
                detections = qr_detector.detect_qr_codes(self.latest_frame)
                if detections:
                    annotated = qr_detector.draw_overlay(self.latest_frame, detections)
                    self.processed_pub.publish(
                        self.bridge.cv2_to_imgmsg(annotated, encoding='bgr8'))

                    goal_handle.succeed()
                    result = ExecuteTest.Result()
                    result.success = True
                    result.result_json = json.dumps({'qr_text': detections[0].data})
                    self.get_logger().info(f'QR detectado: {detections[0].data}')
                    return result

            feedback = ExecuteTest.Feedback()
            feedback.feedback_json = json.dumps({'status': 'buscando QR...'})
            goal_handle.publish_feedback(feedback)
            time.sleep(poll_interval)

        goal_handle.abort()
        result = ExecuteTest.Result()
        result.success = False
        result.result_json = json.dumps({'reason': 'timeout'})
        return result


def main(args=None):
    rclpy.init(args=args)
    node = InspectionPipeline()
    executor = MultiThreadedExecutor()
    executor.add_node(node)
    try:
        executor.spin()
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
