#include <Servo.h>
#include <DHT.h>

// Sensor de Gas
#define MQ_DO A3
#define MQ_AO A0

// Sensor de temperatura/humedad
#define DHT_PIN A5
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

Servo servo_yB;

// definicion de los puertos de entrada 
// control remoto 
int rcPins[6] = {25,29,27,31,33,35};

// definicion de salidas para control de los relevadores
int izq1=10;
int izq2=9;
int der1=15;
int der2=16;

// parametros enviados desde python
// Variables de distancia minima a paredes 
int LL = 25;                        // Distancia lateral
int LD = 25;                        // Distancia delantera

// Potencias para motores 
int potenciaI_frente = 100;
int potenciaD_frente = potenciaI_frente;
int potenciaI_giro = 200;
int potenciaD_giro = potenciaI_giro;
int potenciaI = 0;
int potenciaD = 0;

bool ultimoModoManual = false; // Estado de la palanca para cambiar de Manual a Autonomo (false = autonomo , true = manual)
bool lidarActivo = false; // Estado actual del Lidar

// Variables para guardar lecturas de sensores
int mq_digital = 0;
float mq_analogico = 0.0;
float temperatura = 0.0;
float humedad = 0.0;

//int servo_pinyB = 41;
// definicion de servos para la camara
//Servo serH;
//Servo serV;
//int hori = 13;
//int verti = 12;

float chValue[6];

//pines para control de velocidad de arranque 
int PWM_PINI = 6;
int PWM_PIND = 7;

/* Ultrasónicos  */
// conexiones de triger y echo Izquierdo trasero 
const int trig_it = 41;           
const int echo_it = 42;
// conexiones de triger y echo Izquierdo delantero
const int trig_id = 5;
const int echo_id = 4;
// conexiones de triger y echo delantero izquierdo
const int trig_fi = 19;
const int echo_fi = 20;
// conexiones de triger y echo delantero derecho
const int trig_fd = 8;
const int echo_fd = 40;
// conexiones de triger y echo derecho delantero
const int trig_dd = 21;
const int echo_dd = 22;
// conexiones de triger y echo derecho trasero
const int trig_dt = 23;
const int echo_dt = 24;

// variables para sensores ultrasonicos 
long MIT,MID,MFI,MFD,MDD,MDT; // Mediciones de sensores ultrasonicos en centimetros 
int LIT,LID,LFI,LFD,LDD,LDT; // Valores logicos de lectura de sensores ultrasonicos
 
const int pulseInDelay = 30000;   //20000;

// long potenciaAutomaticaMaxima = 1;
// long potenciaAutomatica = 0.8;
 
void setup() 
{ 
  Serial.begin(9600);
  while (!Serial); // Espera conexión 
  //Serial.println("Arduino listo");
  //salidas a relevadores para movimiento  
  pinMode(izq1, OUTPUT);
  pinMode(izq2, OUTPUT);
  pinMode(der1, OUTPUT);
  pinMode(der2, OUTPUT);

  // salidas PWM para control de velocidad
  pinMode(PWM_PINI,OUTPUT);
  pinMode(PWM_PIND,OUTPUT);

  // Configuración de pines ultrasonico
  pinMode(trig_it, OUTPUT);
  pinMode(echo_it, INPUT);
  pinMode(trig_id, OUTPUT);
  pinMode(echo_id, INPUT);
  pinMode(trig_fi, OUTPUT);
  pinMode(echo_fi, INPUT);
  pinMode(trig_fd, OUTPUT);
  pinMode(echo_fd, INPUT);
  pinMode(trig_dd, OUTPUT);
  pinMode(echo_dd, INPUT);
  pinMode(trig_dt, OUTPUT);
  pinMode(echo_dt, INPUT);

  // configuracion de pines sensores gas/temperatura
  pinMode(MQ_DO, INPUT);
  pinMode(MQ_AO, INPUT);
  dht.begin();

  // ???????????????????
  //serH.attach(hori);
  //serV.attach(verti);

  //servo_yB.attach(servo_pinyB);
}

// Función para mapear un valor de un rango a otro rango
float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  // Convierte el valor 'x' del rango [in_min, in_max] al rango [out_min, out_max]
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Función para leer el valor de un canal
void readChannel(int channel)
{
  // Lee la duración del pulso (en microsegundos) en el pin especificado para el canal dado
  // 'rcPins' es un array que contiene los pines de entrada para cada canal
  // 'HIGH' especifica que estamos midiendo la duración del pulso alto
  // 'pulseInDelay' es el tiempo máximo que esperamos a que se complete el pulso
  int rawValue = pulseIn(rcPins[channel], HIGH, pulseInDelay);
  
  // Convierte el valor leído (rawValue) de un rango de 1000 a 2000 microsegundos a un rango de 0.0 a 1.0
  chValue[channel] = fmap((float)rawValue, 1000.0, 2000.0, 0.0, 1.0);
  
  // Si el valor convertido es menor que 0.0, lo ajusta a 0.0
  chValue[channel] = chValue[channel] < 0.0 ? 0.0 : chValue[channel];
  
  // Si el valor convertido es mayor que 1.0, lo ajusta a 1.0
  chValue[channel] = chValue[channel] > 1.0 ? 1.0 : chValue[channel];
  
}

void leer_sensores_multi()
{
  // Lee sensor de gas
  mq_digital   = digitalRead(MQ_DO);
  mq_analogico = analogRead(MQ_AO);

  // Lee temperatura y humedad
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Verifica que la lectura sea válida
  if (!isnan(t)) temperatura = t;
  if (!isnan(h)) humedad = h;

  // Imprime en monitor serial
  Serial.print("{");
  Serial.print("\"GAS_D\":");  Serial.print(mq_digital);
  Serial.print(",\"GAS_A\":"); Serial.print(mq_analogico);
  Serial.print(",\"TEMP\":");  Serial.print(temperatura);
  Serial.print(",\"HUM\":");   Serial.print(humedad);
  Serial.println("}");
}

void vueltaDerecha()
{
  Serial.println("--------- Alto");
   //Se detiene
  //Izquierdas detenidas
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,LOW);
  //Derechas detenidas
  digitalWrite(der1,LOW);
  digitalWrite(der2,LOW); 
  delay(5000);

  Serial.println("-------->>> inicia giro a la derecha");
  analogWrite(PWM_PINI, potenciaI_giro);
  analogWrite(PWM_PIND, potenciaD_giro);
  //gira derecha
  //Izquierdas adelante
  digitalWrite(izq1,HIGH);
  digitalWrite(izq2,LOW);
  //Derechas atras
  digitalWrite(der1,LOW);
  digitalWrite(der2,HIGH);
  delay(1000);

  Serial.println("--------- Alto antes de girar con sensores");
  //Se detiene
  //Izquierdas detenidas
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,LOW);
  //Derechas detenidas
  digitalWrite(der1,LOW);
  digitalWrite(der2,LOW); 
  delay(5000);

  lee_ultrasonicos();
  
  while(MIT > MID)
    {
      Serial.println("----------->>>> Girando a la derecha");
      //gira derecha
      analogWrite(PWM_PINI, potenciaI_giro);
      analogWrite(PWM_PIND, potenciaD_giro);
      //Izquierdas adelante
      digitalWrite(izq1,HIGH);
      digitalWrite(izq2,LOW);
      //Derechas atras
      digitalWrite(der1,LOW);
      digitalWrite(der2,HIGH);
      lee_ultrasonicos();
    }

  Serial.println("--------- Alto");
   //Se detiene
  //Izquierdas detenidas
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,LOW);
  //Derechas detenidas
  digitalWrite(der1,LOW);
  digitalWrite(der2,LOW); 
  delay(5000);
  
  Serial.println("--------- Adelate luego de la vuelta");
  digitalWrite(izq1,HIGH);
  digitalWrite(izq2,HIGH);
  digitalWrite(der1,HIGH);
  digitalWrite(der2,HIGH); 
  analogWrite(PWM_PINI, potenciaI_frente);
  analogWrite(PWM_PIND, potenciaD_frente);
  delay(1500);  
  Serial.println("T");
}

void vueltaIzquierda()
{

  Serial.println("--------- Alto");
  //Se detiene
  //Izquierdas detenidas
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,LOW);
  //Derechas detenidas
  digitalWrite(der1,LOW);
  digitalWrite(der2,LOW); 
  delay(10000);

  Serial.println("--------->>>>> Inicia giro izquierdo");
  //Inicia giro Izquierdo
  //Izquierdas atras
  analogWrite(PWM_PINI, potenciaI_giro);
  analogWrite(PWM_PIND, potenciaD_giro);
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,HIGH);
  //Derechas adelante
  digitalWrite(der1,HIGH);
  digitalWrite(der2,LOW);
  delay(2000);

  Serial.println("--------- Alto antes de girar con sensores");
  //Se detiene
  //Izquierdas detenidas
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,LOW);
  //Derechas detenidas
  digitalWrite(der1,LOW);
  digitalWrite(der2,LOW); 
  delay(10000);

  lee_ultrasonicos();
  while(MDT > MDD)
    {

      Serial.print("--------->>>>> Girando izquierda");
      Serial.print("MDT ");
      Serial.print(MDT);
      Serial.print("      MDD ");
      Serial.println(MDD);
      //Gira Izquierdo
      //Izquierdas atras
      analogWrite(PWM_PINI, potenciaI_giro);
      analogWrite(PWM_PIND, potenciaD_giro);
      digitalWrite(izq1,LOW);
      digitalWrite(izq2,HIGH);
      //Derechas adelante
      digitalWrite(der1,HIGH);
      digitalWrite(der2,LOW);
      lee_ultrasonicos();
    }
  
  Serial.println("--------- Alto");
  //Se detiene
   //Izquierdas detenidas
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,LOW);
  //Derechas detenidas
  digitalWrite(der1,LOW);
  digitalWrite(der2,LOW); 
  delay(10000); 
   
  Serial.println("--------- Adelate luego de la vuelta");
  digitalWrite(izq1,HIGH);
  digitalWrite(izq2,HIGH);
  digitalWrite(der1,HIGH);
  digitalWrite(der2,HIGH); 
  analogWrite(PWM_PINI, potenciaI_frente);
  analogWrite(PWM_PIND, potenciaD_frente);
  delay(1000);  
  Serial.println("T");

}

void vueltaU()
{

  Serial.println("--------- Alto");
  //Se detiene
  //Izquierdas detenidas
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,LOW);
  //Derechas detenidas
  digitalWrite(der1,LOW);
  digitalWrite(der2,LOW); 
  delay(1000);

  Serial.println("--------->>>>> Inicia giro U");
  //Inicia giro Izquierdo
  //Izquierdas atras
  analogWrite(PWM_PINI, potenciaI_giro);
  analogWrite(PWM_PIND, potenciaD_giro);
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,HIGH);
  //Derechas adelante
  digitalWrite(der1,HIGH);
  digitalWrite(der2,LOW);
  delay(4000);

  Serial.println("--------- Alto antes de girar U con sensores");
  //Se detiene
  //Izquierdas detenidas
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,LOW);
  //Derechas detenidas
  digitalWrite(der1,LOW);
  digitalWrite(der2,LOW); 
  delay(1000);

  lee_ultrasonicos();
  while(MDT > MDD)
    {

      Serial.print("--------->>>>> Girando U");
      Serial.print("MDT ");
      Serial.print(MDT);
      Serial.print("      MDD ");
      Serial.println(MDD);
      //Gira Izquierdo
      //Izquierdas atras
       analogWrite(PWM_PINI, potenciaI_giro);
      analogWrite(PWM_PIND, potenciaD_giro);
      digitalWrite(izq1,LOW);
      digitalWrite(izq2,HIGH);
      //Derechas adelante
      digitalWrite(der1,HIGH);
      digitalWrite(der2,LOW);
      lee_ultrasonicos();
    }
  
  Serial.println("--------- Alto");
  //Se detiene
   //Izquierdas detenidas
  digitalWrite(izq1,LOW);
  digitalWrite(izq2,LOW);
  //Derechas detenidas
  digitalWrite(der1,LOW);
  digitalWrite(der2,LOW); 
  delay(1000); 
   
  Serial.println("--------- Adelate luego de la vuelta");
  digitalWrite(izq1,HIGH);
  digitalWrite(izq2,HIGH);
  digitalWrite(der1,HIGH);
  digitalWrite(der2,HIGH); 
  analogWrite(PWM_PINI, potenciaI_frente);
  analogWrite(PWM_PIND, potenciaD_frente);
  delay(1000);  
  Serial.println("T");

}

//decodifica las lecturas del control remoto para controlar los motores  
void control_motores()
{
  bool modoManual = (chValue[4] > .5);

  if (!modoManual) {
    // ── MODO AUTONOMO ──────────────────────────────────────────
    if (ultimoModoManual) {
      Serial.println("CAMBIO A AUTONOMO");
      while (Serial.available()) Serial.read();
    }
    ultimoModoManual = false;

    lee_ultrasonicos();

    // Lee el comando que manda Python
    String mensaje = "";
    if (Serial.available()) {
      mensaje = Serial.readStringUntil('\n');
      mensaje.trim();

      // Revisa si es un mensaje de estado del LIDAR
      if (mensaje == "LIDAR_ON") {
        lidarActivo = true;
        Serial.println("Lidar conectado");
        mensaje = "";
      } else if (mensaje == "LIDAR_OFF") {
        lidarActivo = false;
        Serial.println("Se perdio conexion con el Lidar");
        mensaje = "";
      }
    }

    if (LFD == 0 && LFI == 0) {
      if (LDD == 0 && LID == 1) {
        vueltaIzquierda();
      } else if (LDD == 1 && LID == 0) {
        vueltaDerecha();
      } else {
        Serial.println("------ ALTO EMERGENCIA -------------");
        digitalWrite(izq1, LOW); 
        digitalWrite(izq2, LOW);
        digitalWrite(der1, LOW); 
        digitalWrite(der2, LOW);
      }

    } else if (lidarActivo) {
      // LIDAR activo asi que empieza a seguir sus comandos
      if (mensaje == "F") {
        analogWrite(PWM_PINI, potenciaI_frente);
        analogWrite(PWM_PIND, potenciaD_frente);
        digitalWrite(izq1, HIGH); 
        digitalWrite(izq2, LOW);
        digitalWrite(der1, HIGH); 
        digitalWrite(der2, LOW);
      } else if (mensaje == "L") {
        vueltaIzquierda();
      } else if (mensaje == "R") {
        vueltaDerecha();
      } else if (mensaje == "U") {
        vueltaU();
      } else {
        // Sin comando entonces se detiene
        digitalWrite(izq1, LOW); 
        digitalWrite(izq2, LOW);
        digitalWrite(der1, LOW); 
        digitalWrite(der2, LOW);
      }

    } else {
      // LIDAR desactivado asi que empieza a navegar mediante los ultrasonicos
      if (LIT == 1 && LID == 1 && LDD == 1 && LDT == 1) {
        // Todo despejado avanza
        analogWrite(PWM_PINI, potenciaI_frente);
        analogWrite(PWM_PIND, potenciaD_frente);
        digitalWrite(izq1, HIGH); 
        digitalWrite(izq2, LOW);
        digitalWrite(der1, HIGH); 
        digitalWrite(der2, LOW);
      } else if (LDD == 0 && LID == 1) {
        vueltaIzquierda();
      } else if (LDD == 1 && LID == 0) {
        vueltaDerecha();
      } else {
        // Sin salida para
        digitalWrite(izq1, LOW); 
        digitalWrite(izq2, LOW);
        digitalWrite(der1, LOW); 
        digitalWrite(der2, LOW);
      }
    }

  } else {
    // ── MODO MANUAL ────────────────────────────────────────────
    if (!ultimoModoManual) {
      Serial.println("CAMBIO A MANUAL");
    }
    ultimoModoManual = true;

    Serial.println("MANUAL");
    while (Serial.available()) Serial.read();

    // Motores izquierdos
    if (chValue[1] > .35 && chValue[1] < .65) {
      digitalWrite(izq1, LOW); 
      digitalWrite(izq2, LOW);
      analogWrite(PWM_PINI, 0);
    } else if (chValue[1] > .65) {
      potenciaI = map(chValue[1] * 100, 65, 100, 0, 255);
      analogWrite(PWM_PINI, potenciaI);
      digitalWrite(izq1, HIGH); 
      digitalWrite(izq2, LOW);
    } else if (chValue[1] < .35) {
      potenciaI = map(chValue[1] * 100, 35, 0, 0, 255);
      analogWrite(PWM_PINI, potenciaI);
      digitalWrite(izq1, LOW); 
      digitalWrite(izq2, HIGH);
    }

    // Motores derechos
    if (chValue[2] > .35 && chValue[2] < .65) {
      digitalWrite(der1, LOW); 
      digitalWrite(der2, LOW);
      analogWrite(PWM_PIND, 0);
    } else if (chValue[2] > .65) {
      potenciaD = map(chValue[2] * 100, 65, 100, 0, 255);
      analogWrite(PWM_PIND, potenciaD);
      digitalWrite(der1, HIGH); 
      digitalWrite(der2, LOW);
    } else if (chValue[2] <= .35) {
      potenciaD = map(chValue[2] * 100, 35, 0, 0, 255);
      analogWrite(PWM_PIND, potenciaD);
      digitalWrite(der1, LOW); 
      digitalWrite(der2, HIGH);
    }
  }
}
//imprime al puerto serial los valores leidos en los canales del control remoto 
void printChannel()
{
  for(int iChannel = 0; iChannel < 6; iChannel++)
  {
    //Serial.print("Ch #");
    //Serial.print(iChannel);
    //Serial.print(": ");
    //Serial.println(chValue[iChannel]);
  };
  //Serial.println("------------");
  //delay(500);
}


/*  Función para lectura de ultrasónicos  */

long ultra(int trigPin,int echoPin){
  long t=0;
  long d=0;

  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  t=pulseIn(echoPin,HIGH);
  d = t/59;

  return d;
}

void lee_ultrasonicos()
{
  //  Distancia Ultrasonicos en cm 
    //Serial.println("entra a leer");
    MIT = ultra(trig_it,echo_it);
    delay(5);
    MFI = ultra(trig_fi,echo_fi);
    delay(5);
    MDT = ultra(trig_dt,echo_dt);
    delay(5);
    MID = ultra(trig_id,echo_id);
    delay(5);
    MDD = ultra(trig_dd,echo_dd);
    delay(5);
    MFD = ultra(trig_fd,echo_fd);

    //  Valores logicos Ultrasonicos 
    if(MIT>LL){
      LIT=1;
    }else{
      LIT=0;
    }
    if(MID>LL){
      LID=1;
    }else{
      LID=0;
    }
    if(MFI>LD){
      LFI=1;
    }else{
      LFI=0;
    }
    if(MFD>LD){
      LFD=1;
    }else{
      LFD=0;
    }
    //logico Derecho Delatanetro
    if(MDD>LL){
      LDD=1;
    }else{
      LDD=0;
    }
    if(MDT>LL){
      LDT=1;
    }else{
      LDT=0;
    }
    
    // Envía los datos al Python (AGREGAR ESTO)
    enviar_datos_sensores();
}

void enviar_datos_sensores()
{
  // Envía los datos de los sensores en formato JSON al Python
  Serial.print("{");
  Serial.print("\"MIT\":");
  Serial.print(MIT);
  Serial.print(",\"MID\":");
  Serial.print(MID);
  Serial.print(",\"MFI\":");
  Serial.print(MFI);
  Serial.print(",\"MFD\":");
  Serial.print(MFD);
  Serial.print(",\"MDD\":");
  Serial.print(MDD);
  Serial.print(",\"MDT\":");
  Serial.print(MDT);
  Serial.print(",\"LIT\":");
  Serial.print(LIT);
  Serial.print(",\"LID\":");
  Serial.print(LID);
  Serial.print(",\"LFI\":");
  Serial.print(LFI);
  Serial.print(",\"LFD\":");
  Serial.print(LFD);
  Serial.print(",\"LDD\":");
  Serial.print(LDD);
  Serial.print(",\"LDT\":");
  Serial.print(LDT);
  Serial.println("}");
}

void loop()
{
  //lee los canales del control remoto 
  for(int iChannel = 0; iChannel < 6; iChannel++){
    readChannel(iChannel);
  }
  //imprime al puerto serial los valores leidos en los canales del control remoto 
  printChannel();
  //decodifica los valores del control remoto en movimentos de los motores 
  control_motores(); 
  // Imprime en el puerto serial y en la terminal de python los valores de los sensores de gas y temperatura/humedad
  leer_sensores_multi();
 
}