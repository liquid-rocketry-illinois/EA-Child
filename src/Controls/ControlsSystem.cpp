#include "ControlsSystem.h"

void Controls::Init(){
    sensors.InitSensors();

    // ONE-TIME RUNNING CODE
}

void Controls::Update(){
    sensors.Update();

    // CONTROLLER CODE
}