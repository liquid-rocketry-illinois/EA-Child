#pragma once

#include <Arduino.h>
#include "DEBUG_SWITCH.h"
#include "Motors.h"

namespace Tests{
    void Ejection_Test(uint8_t eject_pin = 31);
    void Motor_Test(Servos* test);
};