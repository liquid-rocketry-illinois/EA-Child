#include "Controls/Tests.h"

void Tests::Ejection_Test(uint8_t eject_pin = 31){
    pinMode(eject_pin, OUTPUT);
    Serial.println("Countdown from 60");
    for (int i = 60; i >= 0; i--){
        Serial.println(i);
        delay(1000);
    }
    Serial.println("EJECT");
    digitalWrite(eject_pin, HIGH);
    delay(5000);
    digitalWrite(eject_pin, LOW);
    Serial.println("End Test");
    while(1){;}
}