#include "Controls/Procedures.h"
#include "Controls/Tests.h"

Procedures EAChild;

void setup() {
    // Function that contains the runnning code for setup
    EAChild.MAINSETUP();
}

void loop() {
    // Function that contains the running code for looping (either main procedure or tests)
    if (EAChild.EJECTION() == false){
        EAChild.DRIVE_CONTROLS();
    }
    //Tests::Ejection_Test();
}
