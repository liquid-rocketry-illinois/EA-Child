#pragma once
#include "../Math/Vector3D.h"
#include "../Math/QuaternionMath.h"
#include "../Math/Vector.h"
#include <Servo.h>

#define MOTOR_CT 4

#define MOTOR_PIN_1 33
#define MOTOR_PIN_2 36
#define MOTOR_PIN_3 37
#define MOTOR_PIN_4 15

class Motor{
    private:
    Vector3D state;
    Vector3D StateChange; // Preserve last movement for logging
    Vector3D activeAcceleration;
    Servo device;

    public:

    Motor(int pin);

    bool init();
    Vector3D getState();
    void tare();
    void setState(Vector3D toState);
    String Update();
    bool IsActive();
};

struct Servos{
    // Will change these based on actual positions. Color + highlight for labeling.
    Motor MotorA = Motor(MOTOR_PIN_1); // BROWN with black bottom
    Motor MotorB = Motor(MOTOR_PIN_2); // BROWN with black top
    Motor MotorC = Motor(MOTOR_PIN_3); // GRAY with black bottom
    Motor MotorD = Motor(MOTOR_PIN_4); // GRAY with black top

    bool init();
    void actuate(double angle, bool radians = false);
    bool testMotors();

    static int lastFiveAngles[5];
}; 