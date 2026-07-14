#include "config.h"
#include "SerialComm.h"
#include "Maneuvers.h"
#include "UltrasonicArray.h"
#include "motors.h"
#include "RemoteControl.h"

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
    control.update();
    
    if (control.isAutonoumusMode()) {
        ultraSensors.update();
        commu.sendSensorData(&ultraSensors);
        if (ultraSensors.isFLL == 1 && ultraSensors.isFRL == 1) {
            String mensaje = commu.receiveCommand();
            
            if (mensaje == "F") {
                motors.move(speedLeftFront, speedRightFront)
            } else if (mensaje == "L") {
                comunicacion.sendLog("Giro Izquierda");
                maniobras.turnLeft();
                comunicacion.sendStatusDone();
            } else if (mensaje == "R") {
                comunicacion.sendLog("Giro Derecha");
                maniobras.turnRight();
                comunicacion.sendStatusDone();
            } else if (mensaje == "U") {
                comunicacion.sendLog("Giro en U");
                maniobras.uTurn();
                comunicacion.sendStatusDone();
            } else if (mensaje == "S") {
                motors.stop();
            }
        } else {
            if (ultraSensors.isFLL == 0 && ultraSensors.isFRL == 0) {
                if (ultraSensors.isLFL == 1 && ultraSensors.isRFL == 0){
                    comunicacion.sendLog("Giro izquierdo (con ultrasonicos)");
                    maniobras.turnLeft();
                } else if (ultraSensors.isLFL == 0 && ultraSensors.isRFL == 1) {
                    comunicacion.sendLog("Giro derecho (con ultrasonicos)");
                    maniobras.turnRight();
                } else {
                    motors.stop();
                    comunicacion.sendLog("----------------- Alto Emergencia --------------");
                }
            } 
        }
    } else {
        motores.move(control.getLeftTargetSpeed(), control.getRightTargetSpeed());
    }
}