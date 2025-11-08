#include "Controls/Tests.h"

void Tests::Ejection_Test(Servos* test, uint8_t eject_pin = 31){
    pinMode(eject_pin, OUTPUT);
#ifdef MODE_TESTING
    Serial.println("Countdown from 60");
#endif
    for (int i = 60; i >= 0; i--){
#ifdef MODE_TESTING
        Serial.println(i);
#endif
        delay(1000);
    }
#ifdef MODE_TESTING
    Serial.println("EJECT");
#endif
    test->setStateTo({10,10,10,10});
    delay(100);
    test->setStateTo({-10,-10,-10,-10});
    delay(100);
    test->setStateTo({15,15,15,15});
    delay(100);
    test->setStateTo({0,0,0,0});
    delay(500);
    digitalWrite(eject_pin, HIGH);
    delay(5000);
    digitalWrite(eject_pin, LOW);
#ifdef MODE_TESTING
    Serial.println("End Test");
#endif
    while(1){;}
}

void Tests::Motor_Test(Servos* test){
    test->setStateTo({10,10,10,10});
    delay(1000);
    test->setStateTo({-10,-10,-10,-10});
    delay(1000);
}