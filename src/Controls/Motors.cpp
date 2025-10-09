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
    return true;
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

// Set 'radians' to true if your input angle is in radians
void Servos::actuate(double angle, bool radians = false){
    int a;
    if (radians){
        a = int(angle * 180/PI);
    }
    else a = (int)angle;

    MotorA.setState(Vector3D(a, 0, 0));
    MotorB.setState(Vector3D(a, 0, 0));
    MotorC.setState(Vector3D(a, 0, 0));
    MotorD.setState(Vector3D(a, 0, 0));

    for (int i = 4; i > 0; i--){
        lastFiveAngles[i-1] = lastFiveAngles[i];
    }
    lastFiveAngles[4] = a;
}

bool Servos::testMotors(){
    MotorA.setState(Vector3D(-20, 0, 0));
    MotorB.setState(Vector3D(-20, 0, 0));
    MotorC.setState(Vector3D(-20, 0, 0));
    MotorD.setState(Vector3D(-20, 0, 0));

    return true; // Was going to use but now its whatever
}
