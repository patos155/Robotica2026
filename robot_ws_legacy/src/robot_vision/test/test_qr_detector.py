"""
Test de la logica pura de deteccion de QR - no necesita ROS2 corriendo,
solo pytest. Genera un QR en memoria con la libreria qrcode (dev-only,
no es dependencia de runtime del paquete) para no depender de un
archivo de imagen externo en el repo.
"""
import numpy as np
import pytest

from robot_vision.detectors import qr_detector


def _make_qr_frame(text: str) -> np.ndarray:
    qrcode = pytest.importorskip('qrcode')
    import cv2

    img = qrcode.make(text).convert('RGB')
    frame = cv2.cvtColor(np.array(img), cv2.COLOR_RGB2BGR)
    return frame


def test_detects_known_qr_text():
    frame = _make_qr_frame('VICTIMA-01')
    detections = qr_detector.detect_qr_codes(frame)

    assert len(detections) == 1
    assert detections[0].data == 'VICTIMA-01'
    assert len(detections[0].polygon) == 4


def test_no_detections_on_blank_frame():
    blank = np.zeros((200, 200, 3), dtype=np.uint8)
    assert qr_detector.detect_qr_codes(blank) == []


def test_draw_overlay_does_not_mutate_original_frame():
    frame = _make_qr_frame('VICTIMA-02')
    original = frame.copy()

    detections = qr_detector.detect_qr_codes(frame)
    annotated = qr_detector.draw_overlay(frame, detections)

    assert np.array_equal(frame, original)
    assert not np.array_equal(annotated, original)
