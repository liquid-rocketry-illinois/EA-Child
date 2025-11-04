#include "Sensors/IMU_Sensors.h"
#include "SDCard.h"
#include "Math/HelpersFunctions.h"

GNCData::GNCData(ICM42688* secIMU, BNO08x* mainIMU)
    : Secondary(secIMU), Main(mainIMU){
        pyyr->setX(0);
        pyyr->setY(0);
        pyyr->setZ(0);
        U->setX(0);
        U->setY(0);
        U->setZ(0);
    }

GNCData::~GNCData() {}

void IMUSensors::setReports() {
    MainIMU.enableAccelerometer();
    Helpers::delayMS(30);
    MainIMU.enableMagnetometer();
    Helpers::delayMS(30);
    MainIMU.enableGyro();
    Helpers::delayMS(30);
    (MainIMU.enableRotationVector(4));// Serial.println("Output pitch yaw roll");
    Helpers::delayMS(100);
}

void IMUSensors::Init(){
    // Manually call BNO reset and put into SPI mode
    pinMode(MAIN_IMU_PS0, OUTPUT);
    pinMode(MAIN_IMU_PS1, OUTPUT);
    pinMode(MAIN_IMU_RST, OUTPUT);

    digitalWrite(MAIN_IMU_PS0, HIGH); // PS0 and PS1 == HIGH in order to set as SPI communication
    digitalWrite(MAIN_IMU_PS1, HIGH);
    digitalWrite(MAIN_IMU_RST, LOW); // Reset BNO

    // Using Secondary IMU init as in-between time to give BNO time to reset
    if (!SecondaryIMU.begin()){
        Serial.println("Failed to find ICM42688 chip");
    }
    else{
        Serial.println("ICM42688 Found!");
        SecStatus = true;

        // setting the accelerometer full scale range to +/-8G
        SecondaryIMU.setAccelRange(ICM42688::ACCEL_RANGE_8G);
        // setting the gyroscope full scale range to +/-500 deg/s
        SecondaryIMU.setGyroRange(ICM42688::GYRO_RANGE_500DPS);
        // setting DLPF bandwidth to 20 Hz
        SecondaryIMU.setDlpfBandwidth(ICM42688::DLPF_BANDWIDTH_20HZ);

        SecondaryIMU.calibrateAccel();
        SecondaryIMU.calibrateGyro();
    }

    // Turn BNO back on, wait 300ms to reboot
    digitalWrite(MAIN_IMU_RST, HIGH);
    Helpers::delayMS(300);

    if (!MainIMU.beginSPI(MAIN_IMU_CS, MAIN_IMU_INT, MAIN_IMU_RST)) {
        Serial.println("Failed to find BNO08x chip");
        digitalWrite(MAIN_IMU_PS0, LOW);
        digitalWrite(MAIN_IMU_PS1, LOW);
    }
    else{
        Serial.println("BNO08x Found!");
        MainStatus = true;
        MainIMU.softReset();
        Helpers::delayMS(500);

        setReports();
        Helpers::delayMS(50);
        MainIMU.clearTare();
        MainIMU.tareNow();
        Helpers::delayMS(50);

        MainIMU.saveTare();

        
    } 
    
    IMUSensors::Data = GNCData(&SecondaryIMU, &MainIMU);
    
}

Vector3D* IMUSensors::directPYR(Vector3D* accel, Vector3D* gyro, Vector3D* mag, double dt) {
    static Vector3D ypr = Vector3D(0,0,0);

    // --- Step 1: Calculate pitch and roll from accelerometer ---
    float pitchAcc = atan2f(-accel->getX(), sqrtf(accel->getY() * accel->getY() + accel->getZ() * accel->getZ()));
    float rollAcc  = atan2f(accel->getY(), accel->getZ());

    // --- Step 2: Integrate gyro data ---
    ypr.setY(ypr.getY() + (gyro->getX() * dt)); // PITCH
    ypr.setZ(ypr.getZ() + (gyro->getY() * dt)); // ROLL
    ypr.setX(ypr.getX() + (gyro->getZ() * dt))   ; // YAW

    // --- Step 3: Fuse accel angles with gyro (complementary filter) ---
    const float alpha = 0.98f;  // 0.98 = trust gyro more, 0.02 = trust accel more
    ypr.setY(alpha * ypr.getY() + (1.0f - alpha) * pitchAcc);
    ypr.setZ(alpha * ypr.getZ()  + (1.0f - alpha) * rollAcc);

    // --- Step 4: Compute yaw from magnetometer and tilt-compensate ---
    float sinPitch = sinf(ypr.getY());
    float cosPitch = cosf(ypr.getY());
    float sinRoll  = sinf(ypr.getZ());
    float cosRoll  = cosf(ypr.getZ());

    // Tilt compensation
    float Xh = mag->getX() * cosPitch + mag->getZ() * sinPitch;
    float Yh = mag->getX() * sinRoll * sinPitch + mag->getY() * cosRoll - mag->getZ() * sinRoll * cosPitch;
    ypr.setX(atan2f(-Yh, Xh));

    // Optional: convert radians → degrees
    ypr.setY(ypr.getY() * 180.0f / PI);
    ypr.setZ(ypr.getZ() * 180.0f / PI);
    ypr.setX(ypr.getX() * 180.0f / PI); 
    // Normalize yaw to [0, 360)
    if (ypr.getX() < 0) ypr.setX(ypr.getX() + 360.0f); 

    return &ypr;
}

String IMUSensors::Update(){
    static unsigned long beforeBNO;
    static unsigned long afterBNO;
    static Vector3D prevOmega = Vector3D(0,0,0);

    beforeBNO = millis();
    if (MainIMU.wasReset()) {
        Serial.println("sensor was reset "); // COMMENT OUT WHEN DONE
        setReports();
    }

    // -------- MAIN ----------
    if (MainIMU.getSensorEvent()) {
        uint8_t sensestate = MainIMU.getSensorEventID();
        //Serial.println(sensestate);

        if(sensestate == SENSOR_REPORTID_ROTATION_VECTOR){
            //Serial.println(MainIMU.getPitch() * 180.0 / PI);
            //Serial.println(MainIMU.getYaw() * 180.0 / PI);
            //Serial.println(MainIMU.getRoll() * 180.0 / PI);
            Data.quats->i = MainIMU.getQuatI();
            Data.quats->j = MainIMU.getQuatJ();
            Data.quats->k = MainIMU.getQuatK();
            Data.quats->r = MainIMU.getQuatReal();

            Data.pyyr->setX(MainIMU.getPitch() * 180.0 / PI);
            Data.pyyr->setY(MainIMU.getYaw() * 180.0 / PI);
            Data.pyyr->setZ(MainIMU.getRoll() * 180.0 / PI);
        }
        
        if (sensestate == SENSOR_REPORTID_GYROSCOPE_CALIBRATED){
            Data.omega->setX(MainIMU.getGyroX());
            Data.omega->setY(MainIMU.getGyroY());
            Data.omega->setZ(MainIMU.getGyroZ());

            static unsigned long prevMicros = 0;
            unsigned long currMicros = micros();
            if (prevMicros > 0) {
                float dt = (long)(currMicros - prevMicros) / 1.0e6f;
                if (dt > 0) {
                    // Low pass filtering to smooth data
                    Data.alpha->setX(0.2f * ((MainIMU.getGyroX() - prevOmega.getX()) / dt) + (1 - 0.2f) * Data.alpha->getX());
                    Data.alpha->setY(0.2f * ((MainIMU.getGyroY() - prevOmega.getY()) / dt) + (1 - 0.2f) * Data.alpha->getY());
                    Data.alpha->setZ(0.2f * ((MainIMU.getGyroZ() - prevOmega.getZ()) / dt) + (1 - 0.2f) * Data.alpha->getZ());
                }
            }
            prevOmega.setX(MainIMU.getGyroX());
            prevOmega.setY(MainIMU.getGyroY());
            prevOmega.setZ(MainIMU.getGyroZ());
            prevMicros = currMicros;
        }

        if (sensestate == SENSOR_REPORTID_ACCELEROMETER){
            Data.A->setX(MainIMU.getAccelX());
            Data.A->setY(MainIMU.getAccelY());
            Data.A->setZ(MainIMU.getAccelZ());
        }

        if (sensestate == SENSOR_REPORTID_MAGNETIC_FIELD){
            Data.Magnet->setX(MainIMU.getMagX());
            Data.Magnet->setY(MainIMU.getMagY());
            Data.Magnet->setZ(MainIMU.getMagZ());
        }

        if (sensestate == SENSOR_REPORTID_GAME_ROTATION_VECTOR){

        }
    }
    afterBNO = millis();


    // -------- SECONDARY ---------
/*
    if (SecondaryIMU.readSensor() == 1){
        Serial.print(SecondaryIMU.getAccelX_mss(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getAccelY_mss(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getAccelZ_mss(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getGyroX_rads(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getGyroY_rads(),3);
        Serial.print("\t");
        Serial.print(SecondaryIMU.getGyroZ_rads(),3);
        Serial.print("\t");
        Serial.println(SecondaryIMU.getTemperature_C(),2);
    }
    else Serial.println("Secondary IMU Data Fail!");
    */
   
    char buffer[400];
    snprintf(buffer, sizeof(buffer),
        "%lu|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%lu",
        beforeBNO,
        Data.Magnet->getX(), Data.Magnet->getY(), Data.Magnet->getZ(),
        Data.pyyr->getX(), Data.pyyr->getY(), Data.pyyr->getZ(),
        Data.A->getX(), Data.A->getY(), Data.A->getZ(),
        Data.omega->getX(), Data.omega->getY(), Data.omega->getZ(),
        Data.alpha->getX(), Data.alpha->getY(), Data.alpha->getZ(),
        afterBNO
    );
    //Serial.println(buffer);
    return String(buffer);
}