#include "Controls/Motors.h"

Motor::Motor(int pin){
    device.attach(pin);
}

void Motor::tare(){
    state = 0;
}

bool Motor::init(){
    pinMode(MOTOR_PIN_1, OUTPUT);
    pinMode(MOTOR_PIN_2, OUTPUT);
    pinMode(MOTOR_PIN_3, OUTPUT);
    pinMode(MOTOR_PIN_4, OUTPUT);

    delay(50);

    tare();
    return true;
}


void Motor::setState(int toState){
    device.write(toState);
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
#ifdef MODE_TESTING
        Serial.println("Failed to init motor A");
#endif
        return false;
    }
    if(!MotorB.IsActive()){
#ifdef MODE_TESTING
        Serial.println("Failed to init motor B");
#endif
        return false;
    }
    if(!MotorC.IsActive()){
#ifdef MODE_TESTING
        Serial.println("Failed to init motor C");
#endif
        return false;
    }
    if(!MotorD.IsActive()){
#ifdef MODE_TESTING
        Serial.println("Failed to init motor D");
#endif
        return false;
    }
    return true;
}

// toState = struct {A, B, C, D} for all four angles
void Servos::actuate(toState r){
    MotorA.setState(r.A);
    MotorB.setState(r.B);
    MotorC.setState(r.C);
    MotorD.setState(r.D);

    for (int i = 4; i > 0; i--){
        lastFiveAngles[i-1] = lastFiveAngles[i];
    }
    lastFiveAngles[4] = {r.A, r.B, r.C, r.D};
}
