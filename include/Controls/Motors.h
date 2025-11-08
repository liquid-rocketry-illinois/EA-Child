#pragma once
#include "../Math/Vector3D.h"
#include "../Math/QuaternionMath.h"
#include "../Math/Vector.h"
#include <Servo.h>
#include "DEBUG_SWITCH.h"

#define MOTOR_CT 4

#define MOTOR_PIN_1 33
#define MOTOR_PIN_2 36
#define MOTOR_PIN_3 37
#define MOTOR_PIN_4 15

class Motor{
    private:
    int state;
    int StateChange; // Preserve last movement for logging
    Servo device;

    public:

    Motor(int pin);

    bool init();
    void tare();
    void setState(int toState);
    String Update();
    bool IsActive();
};

struct Servos{
    struct toState{
        int A;
        int B;
        int C;
        int D;
    };

    const toState origin = {110, 124, 105, 125};
    
    struct previousAngles{
        int a;
        int b;
        int c;
        int d;
    };

    // Will change these based on actual positions. Color + highlight for labeling.
    Motor MotorA = Motor(MOTOR_PIN_1); // BROWN with black bottom
    Motor MotorB = Motor(MOTOR_PIN_2); // BROWN with black top
    Motor MotorC = Motor(MOTOR_PIN_3); // GRAY with black bottom
    Motor MotorD = Motor(MOTOR_PIN_4); // GRAY with black top

    bool init();
    void setStateTo(toState r);
    bool testMotors();

    previousAngles aa;
    previousAngles bb;
    previousAngles cc;
    previousAngles dd;
    previousAngles ee;

    previousAngles lastFiveAngles[5] = {aa, bb, cc, dd, ee};
}; 