import cv2
import numpy as np
from pyzbar.pyzbar import decode

def Qr():
    leftCap = cv2.VideoCapture(0)
    rightCap = cv2.VideoCapture(1)

    # Configurar una resolucion moderada para no saturar el bus USB
    for cap in [leftCap, rightCap]:
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

    # Verificar si ambas camaras se abrieron correctamente
    if not leftCap.isOpened() or not rightCap.isOpened():
        print("Error: No se pudieron abrir ambas camaras.")
        print(f"Camara Izq Abierta: {leftCap.isOpened()}")
        print(f"Camara Der Abierta: {rightCap.isOpened()}")
        return

    print("Camaras duales iniciadas. Presiona 'q' para salir")

    while True:
        # LECTURA DE DATOS
        ret_izq, frame_izq = leftCap.read()
        ret_der, frame_der = rightCap.read()

        # Si alguna falla, rompemos el bucle
        if not ret_izq or not ret_der:
            print("Error al capturar video de alguna de las camaras")
            break

        # PROCESAMIENTO DE VISION
        imgToProcess = [frame_izq, frame_der]

        for frame in imgToProcess:
            # Detectar QRs en el fotograma actual con PyZbar
            detectedCodes = decode(frame)

            for code in detectedCodes:
                data = code.data.decode('utf-8')
                points = code.polygon
                
                if len(points) == 4:
                    # Dibujar contorno verde
                    pts = np.array(points, np.int32)
                    pts = pts.reshape((-1, 1, 2))
                    cv2.polylines(frame, [pts], True, (0, 255, 0), 3)

                    # Colocar texto
                    posicion_texto = (points[0].x, points[0].y - 10)
                    cv2.putText(frame, data, posicion_texto, cv2.FONT_HERSHEY_SIMPLEX, 
                                0.7, (0, 0, 0), 4)
                    cv2.putText(frame, data, posicion_texto, cv2.FONT_HERSHEY_SIMPLEX, 
                                0.7, (255, 0, 0), 2)

                    # Imprimir en consola de qué lado se detecto (opcional para depurar)
                    lado = "Izquierdo" if frame is frame_izq else "Derecho"
                    print(f"QR Detectado en lado {lado}: {data}")

        #VISUALIZACIÓN PANORÁMICA
        # 'hconcat' pega imagenes horizontalmente. Deben tener la misma altura
        panorama = cv2.hconcat([frame_izq, frame_der])

        # Mostrar la ventana unica panoramica
        cv2.imshow("Vision Panoramica del Robot", panorama)

        # Romper el bucle si se presiona la tecla 'q'
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    leftCap.release()
    rightCap.release()
    cv2.destroyAllWindows()

Qr()