import cv2

def motion():
    # Inicializar la amara
    cap = cv2.VideoCapture(0)
    cap2 = cv2.VideoCapture(1)
    
    # Configurar resolucion a 640x480 para ambas camaras 
    # Esto es crucial para no saturar el bus USB de la computadora a bordo
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    cap2.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap2.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    
    # history: Cuantos fotogramas recuerda para definir que es "fondo" estatico
    # varThreshold: Que tan drastico debe ser el cambio para considerarlo movimiento
    sustractor = cv2.createBackgroundSubtractorMOG2(history=100, varThreshold=40, detectShadows=False)

    print("Iniciando deteccion de movimiento presiona 'q' para salir")

    while True:
        # Leer ambas camaras al mismo tiempo
        ret, frame = cap.read()
        ret2, frame2 = cap2.read()
        
        if not ret or not ret2:
            print("Error al capturar el video")
            break

        panoramicView = cv2.hconcat([frame, frame2])

        # Aplicar el sustractor a la imagen completa ya unida
        mask = sustractor.apply(panoramicView)

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
            # Asegurarse de dibujarlo en la panoramicView
            cv2.rectangle(panoramicView, (x, y), (x + w, y + h), (0, 255, 0), 3)
            
            # Cambiamos la bandera a verdadero porque encontramos un area suficientemente grande moviendose
            validMotionDetected = True

        # 4. Mostrar el texto en pantalla de forma condicional
        if validMotionDetected:
            # Usar la tecnica del contorno negro para que el texto amarillo resalte en cualquier fondo
            cv2.putText(panoramicView, "Movimiento detectado", (30, 50), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 4) # Borde negro
            cv2.putText(panoramicView, "Movimiento detectado", (30, 50), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 255), 2) # Texto amarillo

        # Mostrar la transmision en vivo con las cajas y textos
        cv2.imshow("Vision del Robot", panoramicView)
        
        # cv2.imshow("mask (Blanco = Movimiento)", mask)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Liberar ambas camaras
    cap.release()
    cap2.release()
    cv2.destroyAllWindows()

motion()