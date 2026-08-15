import serial
import time

# Vector de comandos
linea = [
    "F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F",
    "S","S","S","S",
    "L",
    "F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F","F",
]

# Abrir puerto serial
ser = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)
time.sleep(2)              # Espera reset de Arduino
ser.reset_input_buffer()   # Limpia buffer

# Recorrer el vector
for comando in linea:
    print("Enviando:", comando)

    ser.write((comando + "\n").encode())  # Enviar comando
    t0 = time.time()
    while time.time() - t0 < 2:  # 2s timeout
        linea = ser.readline().decode(errors="ignore").strip()
        if not linea:
            continue
        if linea.startswith("CMD:"):
            print("Arduino:", linea)
            break

ser.close()
print("Ruta terminada")

