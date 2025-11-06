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

    public:
    void MAINSETUP();
    void DRIVE_CONTROLS();
    bool EJECTION();
};