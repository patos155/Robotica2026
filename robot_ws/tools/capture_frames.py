"""Guarda N frames de /camera/image_raw como PNG para armar el dataset hazmat.

Requiere el workspace sourceado (para importar robot_vision) y el launch
corriendo. Uso:

    python3 robot_ws/tools/capture_frames.py --out ~/hazmat_dataset \
        --name poison__d10__front__led

Cada archivo queda como <name>__<W>x<H>__<i>.png, con W y H tomados del frame.
Si ya existe un archivo con ese nombre, se usa el siguiente indice libre en vez
de sobrescribirlo.
"""
import argparse
import sys
import time
from pathlib import Path

import cv2

IMAGE_TOPIC = '/camera/image_raw'
DEFAULT_COUNT = 3
# Segundos sin completar los frames antes de rendirse (launch apagado, QoS, etc).
DEFAULT_TIMEOUT_S = 10.0
# Cada cuanto spin_once regresa para revisar el timeout.
SPIN_PERIOD_S = 0.1
FIRST_INDEX = 1


def frame_path(out_dir, name, frame, index):
    """Ruta <out>/<name>__<W>x<H>__<i>.png con W y H del frame."""
    height, width = frame.shape[:2]
    return Path(out_dir) / f'{name}__{width}x{height}__{index}.png'


def save_frame(out_dir, name, frame, index):
    """Guarda el frame en el primer indice >= index que no exista.

    Regresa (ruta, indice usado).
    """
    path = frame_path(out_dir, name, frame, index)
    while path.exists():
        index += 1
        path = frame_path(out_dir, name, frame, index)
    if not cv2.imwrite(str(path), frame):
        raise OSError(f'no se pudo escribir {path}')
    return path, index


def parse_args(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--out', required=True, help='carpeta destino (se crea si no existe)')
    parser.add_argument('--name', required=True, help='prefijo, ej. poison__d10__front__led')
    parser.add_argument('--count', type=int, default=DEFAULT_COUNT, help='frames a guardar')
    parser.add_argument('--timeout', type=float, default=DEFAULT_TIMEOUT_S,
                        help='segundos maximos esperando frames')
    args = parser.parse_args(argv)
    if args.count < 1:
        parser.error('--count debe ser >= 1')
    return args


def main(argv=None):
    args = parse_args(argv)
    out_dir = Path(args.out).expanduser()
    out_dir.mkdir(parents=True, exist_ok=True)

    # Imports de ROS aqui: la logica de nombres/guardado se prueba sin ROS.
    import rclpy
    from cv_bridge import CvBridge
    from sensor_msgs.msg import Image

    from robot_vision.qos import CAMERA_QOS

    rclpy.init()
    node = rclpy.create_node('capture_frames')
    bridge = CvBridge()
    saved = []
    last_stamp = None
    next_index = FIRST_INDEX

    def on_image(msg):
        nonlocal last_stamp, next_index
        stamp = (msg.header.stamp.sec, msg.header.stamp.nanosec)
        if len(saved) >= args.count or stamp == last_stamp:
            return
        last_stamp = stamp
        frame = bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        path, used = save_frame(out_dir, args.name, frame, next_index)
        next_index = used + 1
        saved.append(path)
        node.get_logger().info(f'guardado {path}')

    node.create_subscription(Image, IMAGE_TOPIC, on_image, CAMERA_QOS)
    deadline = time.monotonic() + args.timeout
    try:
        while len(saved) < args.count and time.monotonic() < deadline:
            rclpy.spin_once(node, timeout_sec=SPIN_PERIOD_S)
    except KeyboardInterrupt:
        pass

    ok = len(saved) == args.count
    summary = f'{len(saved)}/{args.count} frames guardados en {out_dir}'
    if ok:
        node.get_logger().info(summary)
    else:
        node.get_logger().error(f'{summary} (timeout de {args.timeout:g}s: '
                                f'¿esta corriendo el launch y publicando {IMAGE_TOPIC}?)')
    node.destroy_node()
    rclpy.shutdown()
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
