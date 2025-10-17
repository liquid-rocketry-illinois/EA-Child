#pragma once

#include "../Sensors/Sensors.h"
#include <Arduino.h>
//#include <Eigen/Dense> // External library for matrix calcs

struct Controls {
    Sensors* Data;

    void Init(Sensors* sensor);
    String Update();
};

