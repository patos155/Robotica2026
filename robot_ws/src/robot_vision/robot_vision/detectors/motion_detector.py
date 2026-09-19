import cv2

MIN_CONTOUR_AREA = 1000


def create_subtractor():
    # history: cuantos fotogramas recuerda para definir que es "fondo" estatico
    # varThreshold: que tan drastico debe ser el cambio para considerarlo movimiento
    return cv2.createBackgroundSubtractorMOG2(history=100, varThreshold=40, detectShadows=False)


def detect_motion(frame, subtractor):
    """Busca movimiento en `frame` (BGR, numpy array) contra el modelo de
    fondo que mantiene `subtractor`.

    Regresa (annotated_frame, motion_found). Un subtractor recien creado
    tarda unos frames en aprender el fondo -- durante ese arranque puede
    reportar movimiento donde no lo hay, hasta estabilizarse.
    """
    mask = subtractor.apply(frame)

    # Umbralizar y dilatar para limpiar ruido y rellenar huecos en la mascara
    _, mask = cv2.threshold(mask, 200, 255, cv2.THRESH_BINARY)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))
    mask = cv2.dilate(mask, kernel, iterations=2)

    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    annotated = frame.copy()
    motion_found = False
    for contour in contours:
        if cv2.contourArea(contour) < MIN_CONTOUR_AREA:
            continue

        x, y, w, h = cv2.boundingRect(contour)
        cv2.rectangle(annotated, (x, y), (x + w, y + h), (0, 255, 0), 3)
        motion_found = True

    if motion_found:
        cv2.putText(annotated, "Movimiento detectado", (30, 50),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 4)
        cv2.putText(annotated, "Movimiento detectado", (30, 50),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 255), 2)

    return annotated, motion_found
