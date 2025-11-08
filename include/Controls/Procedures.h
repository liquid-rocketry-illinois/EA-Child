#pragma once

#include "Motors.h"
#include "../Sensors/Sensors.h"
#include "../SDCard.h"
#include "ControlsSystem.h"
#include <Arduino.h>
#include "DEBUG_SWITCH.h"

#define DATA_WRITE_FREQUENCY 40 // Hertz

class Procedures{
    private:
    SDCard DataCard_o; 
    Sensors sensors_o;
    Servos motors_o;
    CTRLS Controller_o;

    SDCard* DataCard = &DataCard_o; 
    Sensors* sensors = &sensors_o;
    Servos* motors = &motors_o;
    CTRLS* Controller = &Controller_o;

    double initial_height;

    struct MotorStep {
        Servos::toState values;
        uint32_t duration;
    };

    MotorStep sequence[13] = {
        {{0, 0, 0, 0},   477},
        {{4, 4, 4, 4},   425},
        {{8, 8, 8, 8},   396},
        {{4, 4, 4, 4},   418},
        {{0, 0, 0, 0},   1277},
        {{-2, -2, -2, -2}, 900},
        {{-4, -4, -4, -4}, 1050},
        {{-6, -6, -6, -6}, 1250},
        {{-8, -8, -8, -8}, 1500},
        {{-6, -6, -6, -6}, 1700},
        {{-4, -4, -4, -4}, 1900},
        {{-2, -2, -2, -2}, 2050},
        {{0, 0, 0, 0},   0} // final stop, no need to wait
    };

    uint8_t seqIndex = 0;

    public:
    bool launched = false;

    void MAINSETUP(bool quicksetup = false);
    void DRIVE_CONTROLS(bool LAUNCHED);
    void stopData();
    bool updateMotorSequence();
    bool EJECTION();
    Servos* getServoAddr();
};