#include "..\Controls\Motors.h"

Motor::Motor(int pin){
    device.attach(pin);
}

void Motor::tare(){
    state.setX(0);
    state.setY(0);
    state.setZ(0);
}

bool Motor::init(){
    tare();
}

// State vector: Vector3D(ending position, <unused>>, <unused>)). This may change soon
void Motor::setState(Vector3D toState){
    StateChange = toState;
    device.write((int)toState.getX());
}

bool Motor::IsActive(){
    return device.attached();
}

bool Servos::init(){
    MotorA.init();
    MotorB.init();
    MotorC.init();
    MotorD.init();

    if(!MotorA.IsActive()){
        Serial.println("Failed to init motor A");
        return false;
    }
    if(!MotorB.IsActive()){
        Serial.println("Failed to init motor A");
        return false;
    }
    if(!MotorC.IsActive()){
        Serial.println("Failed to init motor A");
        return false;
    }
    if(!MotorD.IsActive()){
        Serial.println("Failed to init motor A");
        return false;
    }
    return true;
}

bool Servos::testMotors(){
    MotorA.setState(Vector3D(-20, 0, 0));
    MotorB.setState(Vector3D(-20, 0, 0));
    MotorC.setState(Vector3D(-20, 0, 0));
    MotorD.setState(Vector3D(-20, 0, 0));
    return true;
}
