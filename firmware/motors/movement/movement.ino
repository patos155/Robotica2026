#include "config.h"
#include "protocol.h"
#include "src/communication/SerialComm.h"
#include "src/maneuvers/Maneuvers.h"
#include "src/sensors/Ultrasonic/UltrasonicArray.h"
#include "src/motors/Motors.h"
#include "src/remote/RemoteControl.h"
#include "src/system/System.h"

Motors motors;
UltrasonicArray ultraSensors;
Remote rc;
Maneuvers maneu;
Communication commu;
Logger dbg;
bool rcWasLost = false;

unsigned long lastCommandTime = 0;

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

    // ----------------------------- CONTROL PERDIDO -----------------------------------
    // Sin señal del control el robot se detiene; no entra a modo autonomo
    if (rc.isSignalLost()) {
        motors.stop();
        if (!rcWasLost) commu.sendLog("Control remoto perdido, motores detenidos");
        rcWasLost = true;
        return;
    }
    rcWasLost = false;

    // ----------------------------- MODO AUTONOMO -----------------------------------
    if (rc.isAutonomousMode()) {
        if (!LEGACY_PROTOCOL) checkWatchdog();  // por que no en legacy: ver LEGACY_PROTOCOL en config.h
        ultraSensors.update();
        dbg.printChannels(rc);
        commu.sendSensorData(&ultraSensors);
        // Como legacy: basta un sensor frontal libre para seguir escuchando a Python
        if (ultraSensors.isFLL() || ultraSensors.isFRL()) {
            char cmd = commu.receiveCommand();

            // Si llego un comando, actualizamos el watchdog
            if (cmd != '\0') {
                lastCommandTime = millis();
            }

            switch (cmd) {
                case Cmd::cmdForward:
                    motors.move(SPEED_LEFT_FRONT, SPEED_RIGHT_FRONT);
                    break;
                case Cmd::cmdLeft:          // el aviso de fin lo manda Maneuvers
                    commu.sendLog("Giro Izquierda");
                    maneu.turnLeft();
                    break;
                case Cmd::cmdRight:
                    commu.sendLog("Giro Derecha");
                    maneu.turnRight();
                    break;
                case Cmd::cmdUturn:
                    commu.sendLog("Giro en U");
                    maneu.uTurn();
                    break;
                case Cmd::cmdStop:          // receiveCommand() tambien devuelve cmdStop ante un comando desconocido
                    motors.stop();
                    break;
            }
        } else {
            // Ambos sensores frontales bloqueados
            lastCommandTime = millis();
            if (ultraSensors.isLFL() && !ultraSensors.isRFL()) {
                commu.sendLog("Giro izquierdo (con ultrasonicos)");
                maneu.turnLeft();
            } else if (!ultraSensors.isLFL() && ultraSensors.isRFL()) {
                commu.sendLog("Giro derecho (con ultrasonicos)");
                maneu.turnRight();
            } else {
                motors.stop();
                commu.sendLog("----------------- Alto Emergencia --------------");
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
            leftTargetSpeed = -map(ch1 * 100, 35, 0, 0, 255);
        }

        if (ch2 > 0.35f && ch2 < 0.65f) {
            rightTargetSpeed = 0;
        } else if (ch2 >= 0.65f) {
            rightTargetSpeed = map(ch2 * 100, 65, 100, 0, 255);
        } else if (ch2 <= 0.35f) {
            rightTargetSpeed = -map(ch2 * 100, 35, 0, 0, 255);
        }
        motors.move(leftTargetSpeed, rightTargetSpeed);
    }
}

// Solo aplica en modo autonomo; no reinicia nada, solo verifica. El log se manda una vez por
// disparo para no saturar el Serial
void checkWatchdog() {
    static bool fired = false;
    if (millis() - lastCommandTime > CMD_TIMEOUT_MS) {
        motors.stop();
        if (!fired) commu.sendLog("Watchdog: No se recibieron comandos, deteniendo motores");
        fired = true;
    } else {
        fired = false;
    }
}