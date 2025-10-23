/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

// These are the four libraries for the four utilized sensors
#include <MS5611.h>
#include <SensirionI2cSht4x.h>

#include "Controls/Procedures.hpp"
#include "Controls/Tests.hpp"

void setup() {
    // Function that contains the runnning code for setup
    Procedures::___MAINSETUP();
}

void loop() {
    // Function that contains the running code for looping (either main procedure or tests)
    Procedures::___MAIN();
    //Tests::Ejection_Test();
}
