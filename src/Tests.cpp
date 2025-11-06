#include "Controls/Tests.h"

void Tests::Ejection_Test(uint8_t eject_pin = 31){
    pinMode(eject_pin, OUTPUT);
#ifdef MODE_TESTING
    Serial.println("Countdown from 60");
#endif
    for (int i = 60; i >= 0; i--){
        Serial.println(i);
        delay(1000);
    }
#ifdef MODE_TESTING
    Serial.println("EJECT");
#endif
    digitalWrite(eject_pin, HIGH);
    delay(5000);
    digitalWrite(eject_pin, LOW);
#ifdef MODE_TESTING
    Serial.println("End Test");
#endif
    while(1){;}
}

void Tests::Motor_Test(Servos* test){
    test->actuate({108, 118, 100, 117});
    delay(400);
}