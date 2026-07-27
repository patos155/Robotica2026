import cv2

def motion():
    cap = cv2.VideoCapture(0)
    
    # history: Cuantos fotogramas recuerda para definir que es "fondo" estatico
    # varThreshold: Que tan drastico debe ser el cambio para considerarlo movimiento
    sustractor = cv2.createBackgroundSubtractorMOG2(history=100, varThreshold=40, detectShadows=False)

    print("Iniciando deteccion de movimiento presiona 'q' para salir")

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error al capturar el video")
            break

        mask = sustractor.apply(frame)

        # Aplicamos una umbralizacion y luego dilatamos los pixeles blancos para rellenar huecos
        _, mask = cv2.threshold(mask, 200, 255, cv2.THRESH_BINARY)
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))
        mask = cv2.dilate(mask, kernel, iterations=2)

        # Encontrar los contornos de las manchas blancas en la mascara limpia
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        validMotionDetected = False

        for contorn in contours:
            # Filtrar movimientos muy pequeños (como ruido de la camara o vibraciones)
            # Puedes ajustar este valor (1000) dependiendo de que tan cerca esté la camara de la caja
            if cv2.contourArea(contorn) < 1000:
                continue

            # Obtener las coordenadas para dibujar el "bounding box"
            x, y, w, h = cv2.boundingRect(contorn)

            # Dibujar el rectangulo verde sobre el objetivo en movimiento (Grosor 3)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 3)
            
            # Cambiamos la bandera a verdadero porque encontramos un area suficientemente grande moviendose
            validMotionDetected = True

        # 4. Mostrar el texto en pantalla de forma condicional
        if validMotionDetected:
            # Usar la tecnica del contorno negro para que el texto amarillo resalte en cualquier fondo
            cv2.putText(frame, "Movimiento detectado", (30, 50), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 4) # Borde negro
            cv2.putText(frame, "Movimiento detectado", (30, 50), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 255), 2) # Texto amarillo

        # Mostrar la transmision en vivo con las cajas y textos
        cv2.imshow("Vision del Robot", frame)
        
        # cv2.imshow("mask (Blanco = Movimiento)", mask)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()
motion()