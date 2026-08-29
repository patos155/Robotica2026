import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge


class MotionDetector(Node):

    def __init__(self):
        super().__init__('motion_detector')

        self.bridge = CvBridge()

        # history: Cuantos fotogramas recuerda para definir que es "fondo" estatico
        # varThreshold: Que tan drastico debe ser el cambio para considerarlo movimiento
        self.subtractor = cv2.createBackgroundSubtractorMOG2(history=100, varThreshold=40, detectShadows=False)

        self.create_subscription(Image, '/camera/image_raw', self.on_frame, 10)
        self.processed_pub = self.create_publisher(Image, '/motion/image_processed', 10)
        self.get_logger().info('motion_detector iniciado')

    def on_frame(self, msg):
        frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')

        mask = self.subtractor.apply(frame)

        # Aplicamos una umbralizacion y luego dilatamos los pixeles blancos para rellenar huecos
        _, mask = cv2.threshold(mask, 200, 255, cv2.THRESH_BINARY)
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))
        mask = cv2.dilate(mask, kernel, iterations=2)

        # Encontrar los contornos de las manchas blancas en la mascara limpia
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        valid_motion_detected = False
        for contour in contours:
            # Filtrar movimientos muy pequeños (como ruido de la camara o vibraciones)
            if cv2.contourArea(contour) < 1000:
                continue

            # Obtener las coordenadas para dibujar el "bounding box"
            x, y, w, h = cv2.boundingRect(contour)

            # Dibujar el rectangulo verde sobre el objetivo en movimiento (Grosor 3)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 3)

            valid_motion_detected = True

        if valid_motion_detected:
            # Usar la tecnica del contorno negro para que el texto amarillo resalte en cualquier fondo
            cv2.putText(frame, "Movimiento detectado", (30, 50),
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 4)  # Borde negro
            cv2.putText(frame, "Movimiento detectado", (30, 50),
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 255), 2)  # Texto amarillo

        processed_msg = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')
        self.processed_pub.publish(processed_msg)


def main(args=None):
    rclpy.init(args=args)
    node = MotionDetector()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()