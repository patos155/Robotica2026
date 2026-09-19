import cv2
import numpy as np
from pyzbar.pyzbar import decode


def detect_qr(frame):
    """Busca codigos QR en `frame` (BGR, numpy array).

    Regresa (annotated_frame, qr_text). qr_text es '' si no se detecto nada.
    Si hay varios QR en el frame se anotan todos pero solo se regresa el
    texto del primero.
    """
    annotated = frame.copy()
    qr_text = ''

    for code in decode(frame):
        qr_data = code.data.decode('utf-8')
        if not qr_text:
            qr_text = qr_data

        if len(code.polygon) == 4:
            polygon_points = [(p.x, p.y) for p in code.polygon]
            pts = np.array(polygon_points, np.int32).reshape((-1, 1, 2))
            cv2.polylines(annotated, [pts], True, (0, 255, 0), 3)

            text_position = polygon_points[0]
            cv2.putText(annotated, qr_data, text_position, cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)

    return annotated, qr_text
