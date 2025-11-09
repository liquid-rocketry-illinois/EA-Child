#include <SPI.h>
#include <SD.h>
#include "Math/Vector3D.h"
#include "DEBUG_SWITCH.h"

// SD CARD STUFF
const int chipSelect = BUILTIN_SDCARD;

class SDCard{
    private:
    File dataFile;

    public:
    SDCard(){};

    bool init(){
        //Serial.print("Initializing SD card...");

        // see if the card is present and can be initialized:
        if (!SD.begin(chipSelect)) {
            //Serial.println("Card failed, or not present");
            return false;
        }
        else //Serial.println("card initialized.");
        dataFile = SD.open("FLIGHT_LOG.txt", FILE_WRITE);
        return true;
    }

    void SDWrite(const String& dataString){
        // if the file is available, write to it:
        if (dataFile) {
            dataFile.println(dataString);
            // Flush ensures the data actually gets written to the card
            dataFile.flush(); 
        }
        // if the file isn't open, pop up an error:
        else {
            //Serial.println("error opening datalog.txt");
        }
    }

    void close() {
        if (dataFile) {
            dataFile.close();
            //Serial.println("Log file closed.");
        }
    }
};