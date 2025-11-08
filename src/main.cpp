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
    EAChild.MAINSETUP();
}

void loop() {
    // Function that contains the running code for looping (either main procedure or tests)
    //Tests::Motor_Test(EAChild.getServoAddr());
    static bool ejected = false;
    
    if (EAChild.EJECTION() == false){
        EAChild.DRIVE_CONTROLS(EAChild.launched);
    }
    else if (EAChild.EJECTION() == true){
        ejected = true;
    }

    if (ejected){
        static uint32_t start = millis();
        if(millis() - start > 120000) EAChild.stopData();
    }

    //Tests::Ejection_Test(EAChild.getServoAddr());
}
