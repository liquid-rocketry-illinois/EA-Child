#include <SPI.h>
#include <SD.h>
#include "Math/Vector3D.h"

// SD CARD STUFF
const int chipSelect = BUILTIN_SDCARD;

class SDCard{
    public:
    SDCard(){
        Serial.print("Initializing SD card...");

        // see if the card is present and can be initialized:
        if (!SD.begin(chipSelect)) {
            Serial.println("Card failed, or not present");
            // don't do anything more:
            while (1);
        }
        Serial.println("card initialized.");
    };

    void SDWrite(Vector3D MainIMUpyr, Vector3D SecIMUpyr, Vector3D Magnetometer, float Temperature, float Pressure, float Humidity){ // Function to write string to SD card file. FOLLOW INPUTS FOR FORMAT!!
        // convert string for assembling the data to log:
        String mix = (String)MainIMUpyr.getX();    
        String miy = (String)MainIMUpyr.getY();
        String miz = (String)MainIMUpyr.getZ();
        String six = (String)SecIMUpyr.getX();
        String siy = (String)SecIMUpyr.getY();
        String siz = (String)SecIMUpyr.getZ();
        String mx = (String)Magnetometer.getX();
        String my = (String)Magnetometer.getY();
        String mz = (String)Magnetometer.getZ();
        String Temp = (String)Temperature;
        String Pres = (String)Pressure;
        String Humid = (String)Humidity;

        String dataString = mix + " " + miy + " " + miz + " "
                             + six + " " + siy + " " + siz + " "
                              + mx + " " + my + " " + mz + " "
                               + Temp + " "
                                + Pres + " "
                                 + Humid + " ";

        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        File dataFile = SD.open("datalog.txt", FILE_WRITE);

        // if the file is available, write to it:
        if (dataFile) {
            dataFile.println(dataString);
            dataFile.close();
            // print to the serial port too:
            Serial.println(dataString); // for debug
        }
        // if the file isn't open, pop up an error:
        else {
            Serial.println("error opening datalog.txt");
        }
    }
};