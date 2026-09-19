from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy

# Un solo perfil para todos los topics de frames de camara (crudo y
# comprimido): publisher y subscriber deben coincidir en BEST_EFFORT, si no
# el subscriber nunca recibe nada sin ningun error visible.
CAMERA_QOS = QoSProfile(
    reliability=ReliabilityPolicy.BEST_EFFORT,
    history=HistoryPolicy.KEEP_LAST,
    depth=1,
)
