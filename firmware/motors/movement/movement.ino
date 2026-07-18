#include "config.h"
#include "./communication/SerialComm.h"
#include "./maneuvers/Maneuvers.h"
#include "./sensors/Ultrasonic/UltrasonicArray.h"
#include "./motors/Motors.h"
#include "./remote/RemoteControl.h"

Motors motors;
UltrasonicArray ultraSensors;
Remote rc;
Maneuvers maneu;
Communication commu;

void setup() {
    commu.begin(9600);
    commu.sendLog("Iniciando hardware");
    motors.begin();
    ultraSensors.begin();
    rc.begin();
    maneu.begin(&motors,& ultraSensors, &commu);
    commu.sendLog("Hardware listo");
}

void loop() {
    rc.update();
    
    if (rc.isAutonomousMode()) {
        ultraSensors.update();
        commu.sendSensorData(&ultraSensors);
        if (ultraSensors.isFLL() == 1 && ultraSensors.isFRL() == 1) {
            String mensaje = commu.receiveCommand();
            
            if (mensaje == "F") {
                motors.move(speedLeftFront, speedRightFront);
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
        motors.move(rc.getLeftTargetSpeed(), rc.getRightTargetSpeed());
    }
}