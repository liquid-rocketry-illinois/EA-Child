#pragma once
#include <Arduino.h>

namespace Helpers{
    void delayMS(unsigned int ms); // Helper func for delays
}
    
void Helpers::delayMS(unsigned int ms){ unsigned long s = millis(); while (millis() - s < (long)ms) {;} }