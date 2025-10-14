#pragma once

#include "../Sensors/Sensors.h"
//#include <Eigen/Dense> // External library for matrix calcs

namespace Controls {
    Sensors sensors;

    void Init();
    void Update();
}

