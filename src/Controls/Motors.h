#pragma once
#include "../Math/Vector3D.h"
#include "../Math/QuaternionMath.h"
#include "../Math/Vector.h"
#include <Servo.h>

#define MOTOR_CT 4
//Placeholders
#define MOTOR1_A (uint8_t*)0x00
#define MOTOR2_A (uint8_t*)0x01
#define MOTOR3_A (uint8_t*)0x02
#define MOTOR4_A (uint8_t*)0x03

class Motor{
    private:
    Vector3D state;
    Vector3D StateChange; // Preserve last movement for logging
    Vector3D activeAcceleration;
    Servo device;

    public:

    Motor(int pin);
    ~Motor();

    bool init();
    Vector3D getState();
    void tare();
    void setState(Vector3D toState);
    String Update();
    bool IsActive();
};

struct Servos{
    Motor MotorA = Motor(33);
    Motor MotorB = Motor(36);
    Motor MotorC = Motor(37);
    Motor MotorD = Motor(29);

    bool init();
    Vector<size_t, MOTOR_CT>* MotorAddrs;
    void setMotorAddrs();
    void actuate(double angle, bool radians = false);
    bool testMotors();

    static int lastFiveAngles[5];
}; 