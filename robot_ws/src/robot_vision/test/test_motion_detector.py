import cv2
import numpy as np

from robot_vision.detectors.motion_detector import create_subtractor, detect_motion


def test_detect_motion_no_change_after_warmup():
    subtractor = create_subtractor()
    frame = np.zeros((100, 100, 3), dtype=np.uint8)

    motion_found = True
    for _ in range(30):
        _, motion_found = detect_motion(frame, subtractor)

    assert motion_found is False


def test_detect_motion_finds_new_object():
    subtractor = create_subtractor()
    background = np.zeros((100, 100, 3), dtype=np.uint8)

    for _ in range(30):
        detect_motion(background, subtractor)

    frame_with_object = background.copy()
    cv2.rectangle(frame_with_object, (20, 20), (60, 60), (255, 255, 255), -1)

    _, motion_found = detect_motion(frame_with_object, subtractor)
    assert motion_found is True
