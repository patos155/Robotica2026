import cv2
import numpy as np
from pyzbar.pyzbar import decode

def qr():
    cap = cv2.VideoCapture(0)
    
    # Aumentar la resolucion al maximo para ver de lejos
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

    print("Iniciando camara en alta resolucion presiona 'q' para salir")

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        detectedCodes = decode(frame)

        for code in detectedCodes:
            # Extraer el texto del QR
            data = code.data.decode('utf-8')
            
            points = code.polygon
            
            if len(points) == 4:
                pts = np.array(points, np.int32)
                pts = pts.reshape((-1, 1, 2))
                
                # Dibujar el contorno verde
                cv2.polylines(frame, [pts], True, (0, 255, 0), 3)

                # Colocar el texto usando la primera esquina (puntos[0])
                textPosition = (points[0].x, points[0].y - 10)
                
                # Borde negro y texto azul para contraste
                cv2.putText(frame, data, textPosition, cv2.FONT_HERSHEY_SIMPLEX, 
                            0.7, (0, 0, 0), 4)
                cv2.putText(frame, data, textPosition, cv2.FONT_HERSHEY_SIMPLEX, 
                            0.7, (255, 0, 0), 2)

                print(f"QR Lejano Detectado: {data}")

        # Si la ventana de 1920x1080 es muy grande para tu monitor, puedes reescalarla solo para verla
        # frame_mostrar = cv2.resize(frame, (960, 540))
        # cv2.imshow("Vision del Robot", frame_mostrar)
        
        cv2.imshow("Vision del Robot", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()
qr()