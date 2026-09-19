import numpy as np
import qrcode

from robot_vision.detectors.qr_detector import detect_qr


def test_detect_qr_finds_text():
    img = qrcode.make('HELLO123').convert('RGB')
    frame = np.array(img)[:, :, ::-1].copy()  # RGB -> BGR
    _, text = detect_qr(frame)
    assert text == 'HELLO123'


def test_detect_qr_no_code_returns_empty():
    frame = np.zeros((100, 100, 3), dtype=np.uint8)
    _, text = detect_qr(frame)
    assert text == ''
