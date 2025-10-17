#include "ControlsSystem.h"

void Controls::Init(Sensors* sensors){
    this->Data = sensors;
}

String Controls::Update(){
    Data->Update();

    // CONTROLLER CODE



    // END CONTROLLER CODE

    return (String)(12); // Return actuation amount
}