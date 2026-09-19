from robot_vision.image_compressor import NS_PER_S, throttle

SOURCE_INTERVAL_NS = 40_000_000  # camara a 25 fps


def _published(max_fps, seconds=10, start_ns=0):
    interval_ns = int(NS_PER_S / max_fps)
    next_ns = 0
    count = 0
    for i in range(seconds * 25):
        publish, next_ns = throttle(start_ns + i * SOURCE_INTERVAL_NS, next_ns, interval_ns)
        count += publish
    return count


def test_throttle_averages_max_fps_even_if_it_does_not_divide_camera_fps():
    for max_fps in (15, 12, 10, 5):
        assert abs(_published(max_fps) - max_fps * 10) <= 1


def test_throttle_works_with_a_large_monotonic_clock():
    assert abs(_published(12, start_ns=987_654_321_000_000) - 120) <= 1


def test_throttle_does_not_invent_frames_above_camera_fps():
    assert _published(30) == 250


def test_throttle_lets_at_most_one_extra_frame_through_after_a_pause():
    interval_ns = int(NS_PER_S / 12)
    next_ns = 0
    for i in range(25):
        _, next_ns = throttle(i * SOURCE_INTERVAL_NS, next_ns, interval_ns)

    resume_ns = 775 * SOURCE_INTERVAL_NS  # 30 s sin frames
    burst = 0
    for i in range(3):  # los primeros 120 ms tras reanudar
        publish, next_ns = throttle(resume_ns + i * SOURCE_INTERVAL_NS, next_ns, interval_ns)
        burst += publish

    assert burst <= 2
