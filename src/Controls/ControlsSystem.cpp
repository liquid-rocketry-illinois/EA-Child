#pragma once

#include "ControlsSystem.h"

void Controls::Init(){
    sensors.InitSensors();
}

void Controls::Update(){
    sensors.Update();
}