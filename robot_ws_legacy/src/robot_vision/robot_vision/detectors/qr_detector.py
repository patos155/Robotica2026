"""
Deteccion de codigos QR - sin dependencias de ROS2, para poder
probarlo con pytest sin levantar el grafo completo. Basado en el
script original del equipo (cv2 + pyzbar); se separa "detectar" de
"dibujar" para que inspection_pipeline.py decida cuando dibujar
segun si hay una prueba de QR activa o no.
"""
from dataclasses import dataclass

import cv2
import numpy as np
from pyzbar.pyzbar import decode


@dataclass
class QRDetection:
    data: str
    polygon: list  # [(x, y), (x, y), (x, y), (x, y)]


def detect_qr_codes(frame: np.ndarray) -> list[QRDetection]:
    """Busca codigos QR en un frame. No muta el frame ni dibuja nada."""
    detections = []
    for code in decode(frame):
        if len(code.polygon) != 4:
            continue
        detections.append(QRDetection(
            data=code.data.decode('utf-8'),
            polygon=[(p.x, p.y) for p in code.polygon],
        ))
    return detections


def draw_overlay(frame: np.ndarray, detections: list[QRDetection]) -> np.ndarray:
    """Regresa una COPIA del frame con el contorno y el texto dibujados.
    No modifica el frame original: camera_stream.py sigue necesitando
    el frame limpio para otros consumidores (navegacion, otras pruebas)."""
    annotated = frame.copy()
    for det in detections:
        pts = np.array(det.polygon, np.int32).reshape((-1, 1, 2))
        cv2.polylines(annotated, [pts], True, (0, 255, 0), 3)

        text_pos = det.polygon[0]
        # Borde negro + texto azul para contraste, igual que el original
        cv2.putText(annotated, det.data, text_pos, cv2.FONT_HERSHEY_SIMPLEX,
                    0.7, (0, 0, 0), 4)
        cv2.putText(annotated, det.data, text_pos, cv2.FONT_HERSHEY_SIMPLEX,
                    0.7, (255, 0, 0), 2)
    return annotated
