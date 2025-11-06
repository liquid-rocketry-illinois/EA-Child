#include "Controls/Procedures.h"
#include "Controls/Tests.h"

/**
 * @attention EXTREMELY IMPORTANT NOTE!!!!!!!
 * 
 * BEFORE flight, make sure to navigate to DEBUG_SWITCH.h 
 * and turn OFF the define!!!! The rocket WILL NOT fly with 
 * it ON!!!!!
 */

Procedures EAChild;

void setup() {
    // Function that contains the runnning code for setup
    EAChild.MAINSETUP(true);
}

void loop() {
    // Function that contains the running code for looping (either main procedure or tests)
    //Tests::Motor_Test(EAChild.getServoAddr());
/*
    if (EAChild.EJECTION() == false){
        EAChild.DRIVE_CONTROLS();
    }
*/
    Tests::Ejection_Test();
}
