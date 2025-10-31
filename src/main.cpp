// These are the four libraries for the four utilized sensors
#include <MS5611.h>
#include <SensirionI2cSht4x.h>

#include "Controls/Procedures.hpp"
#include "Controls/Tests.hpp"

void setup() {
    // Function that contains the runnning code for setup
    Procedures::___MAINSETUP();
}

void loop() {
    // Function that contains the running code for looping (either main procedure or tests)
    Procedures::___MAIN();
    //Tests::Ejection_Test();
}
