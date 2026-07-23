#include "config.h"
#include "./communication/SerialComm.h"
#include "./maneuvers/Maneuvers.h"
#include "./sensors/Ultrasonic/UltrasonicArray.h"
#include "./motors/Motors.h"
#include "./remote/RemoteControl.h"
#include "./system/System.h"

Motors motors;
UltrasonicArray ultraSensors;
Remote rc;
Maneuvers maneu;
Communication commu;
System dbg;

unsigned long lastCommandTime = 0;
const unsigned long timeOut = 500;

void setup() {
    commu.begin(9600);
    commu.sendLog("Iniciando hardware");
    motors.begin();
    ultraSensors.begin();
    rc.begin();
    maneu.begin(&motors,& ultraSensors, &commu);
    dbg.begin(&commu, false); // deshabilitamos la depuracion del sistema por defecto
    commu.sendLog("Hardware listo");
}

void loop() {
    rc.update();

    int leftTargetSpeed = 0;
    int rightTargetSpeed = 0;

    watchdogReset();

    // ----------------------------- MODO AUTONOMO -----------------------------------
    if (rc.isAutonomousMode()) {
        ultraSensors.update();
        dbg.printChannels(rc);
        commu.sendSensorData(&ultraSensors);
        if (ultraSensors.isFLL() == 1 && ultraSensors.isFRL() == 1) {
            String mensaje = commu.receiveCommand();
            
            // Si el comando no está vacio, actualizamos el watchdog
            if (mensaje.length() > 0) {
                lastCommandTime = millis();
            }
            
            if (mensaje == "F") {
                motors.move(SPEED_LEFT_FRONT, SPEED_RIGHT_FRONT);
            } else if (mensaje == "L") {
                commu.sendLog("Giro Izquierda");
                maneu.turnLeft();
                commu.sendStatusDone();
            } else if (mensaje == "R") {
                commu.sendLog("Giro Derecha");
                maneu.turnRight();
                commu.sendStatusDone();
            } else if (mensaje == "U") {
                commu.sendLog("Giro en U");
                maneu.uTurn();
                commu.sendStatusDone();
            } else if (mensaje == "S") {
                motors.stop();
            }
        } else {
            lastCommandTime = millis();
            if (ultraSensors.isFLL() == 0 && ultraSensors.isFRL() == 0) {
                if (ultraSensors.isLFL() == 1 && ultraSensors.isRFL() == 0){
                    commu.sendLog("Giro izquierdo (con ultrasonicos)");
                    maneu.turnLeft();
                } else if (ultraSensors.isLFL() == 0 && ultraSensors.isRFL() == 1) {
                    commu.sendLog("Giro derecho (con ultrasonicos)");
                    maneu.turnRight();
                } else {
                    motors.stop();
                    commu.sendLog("----------------- Alto Emergencia --------------");
                }
            } 
        }
    } else {
        // ----------------------------- MODO MANUAL -----------------------------------
        dbg.printChannels(rc);
        float ch1 = rc.getChannelValue(1);
        float ch2 = rc.getChannelValue(2);

        if (ch1 > 0.35f && ch1 < 0.65f) {
            leftTargetSpeed = 0;
        } else if (ch1 >= 0.65f) {
            leftTargetSpeed = map(ch1 * 100, 65, 100, 0, 255);
        } else if (ch1 <= 0.35f) {
            leftTargetSpeed = -map(ch1 * 100, 65, 100, 0, 255);
        }

        if (ch2 > 0.35f && ch2 < 0.65f) {
            rightTargetSpeed = 0;
        } else if (ch2 >= 0.65f) {
            rightTargetSpeed = map(ch2 * 100, 65, 100, 0, 255);
        } else if (ch2 <= 0.35f) {
            rightTargetSpeed = -map(ch2 * 100, 65, 100, 0, 255);
        }
        motors.move(leftTargetSpeed, rightTargetSpeed);
    }
}

void watchdogReset() {
    if (millis() - lastCommandTime > timeOut) {
        motors.stop();
        commu.sendLog("Watchdog: No se recibieron comandos, deteniendo motores");
    }
}