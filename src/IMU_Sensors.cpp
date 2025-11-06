#include "Sensors/IMU_Sensors.h"
#include "SDCard.h"

#define IMU_PACKET_TIMEOUT_MS 50
// Rotation vector Report Interval (microsecs)
#define rotationVectorReportIntervalUs 10000

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
    delay(30);
    MainIMU.enableMagnetometer();
    delay(30);
    MainIMU.enableGyro();
    delay(30);
    MainIMU.enableReport(SENSOR_REPORTID_ROTATION_VECTOR, rotationVectorReportIntervalUs);
}

void IMUSensors::Init(){
    // Manually call BNO reset and put into SPI mode
    pinMode(MAIN_IMU_PS0, OUTPUT);
    pinMode(MAIN_IMU_PS1, OUTPUT);
    pinMode(MAIN_IMU_RST, OUTPUT);

    digitalWrite(MAIN_IMU_PS0, HIGH); // PS0 and PS1 == HIGH in order to set as SPI communication
    digitalWrite(MAIN_IMU_PS1, HIGH);
    digitalWrite(MAIN_IMU_RST, LOW); // Reset BNO

    delay(100);

    // Turn BNO back on, wait 300ms to reboot
    digitalWrite(MAIN_IMU_RST, HIGH);
    delay(300);

    if (!MainIMU.beginSPI(MAIN_IMU_CS, MAIN_IMU_INT, MAIN_IMU_RST)) {
#ifdef MODE_TESTING
        Serial.println("Failed to find BNO08x chip");
#endif
        digitalWrite(MAIN_IMU_PS0, LOW);
        digitalWrite(MAIN_IMU_PS1, LOW);
    }
    else{
#ifdef MODE_TESTING
        Serial.println("BNO08x Found!");
#endif
        MainStatus = true;
        MainIMU.softReset();
        delay(500);

        setReports();
        delay(50);
        MainIMU.clearTare();
        MainIMU.tareNow();
        delay(50);

        MainIMU.saveTare();

        
    } 
    IMUSensors::Data = GNCData(&SecondaryIMU, &MainIMU);
    
}

#ifndef isnan
inline bool isnan(float x) { return x != x; }
#endif

#ifndef isinf
inline bool isinf(float x) { return !isnan(x) && isnan(x - x); }
#endif


const char* IMUSensors::Update() {
    static char buffer[540];
    static Vector3D prevOmega(0,0,0);

    // clear flags at start of cycle
    bool gotQuat = false, gotGyro = false, gotAcc = false, gotMag = false;

    unsigned long startMs = millis();
    unsigned long beforeBNO = startMs;

    if (MainIMU.wasReset()) {
#ifdef MODE_TESTING
        Serial.println("sensor was reset");
#endif
        setReports();
        delay(200); // give it a moment
    }

    static unsigned long lastKeepAlive = 0;
    if (millis() - lastKeepAlive > 500) {
        MainIMU.enableReport(SENSOR_REPORTID_ROTATION_VECTOR, rotationVectorReportIntervalUs);
        lastKeepAlive = millis();
    }

    // Collect data set
    while (millis() - startMs < IMU_PACKET_TIMEOUT_MS) {
        if (!MainIMU.getSensorEvent()) { yield(); continue; }

        uint8_t id = MainIMU.getSensorEventID();

        switch (id) {
            case SENSOR_REPORTID_ROTATION_VECTOR:
                Data.quats->i = MainIMU.getQuatI();
                Data.quats->j = MainIMU.getQuatJ();
                Data.quats->k = MainIMU.getQuatK();
                Data.quats->r = MainIMU.getQuatReal();

                Data.pyyr->setX(MainIMU.getPitch() * 180.0f / PI);
                Data.pyyr->setY(MainIMU.getYaw()   * 180.0f / PI);
                Data.pyyr->setZ(MainIMU.getRoll()  * 180.0f / PI);
                gotQuat = true;
                break;

            case SENSOR_REPORTID_GYROSCOPE_CALIBRATED:
                Data.omega->setX(MainIMU.getGyroX());
                Data.omega->setY(MainIMU.getGyroY());
                Data.omega->setZ(MainIMU.getGyroZ());
                gotGyro = true;
                break;

            case SENSOR_REPORTID_ACCELEROMETER:
                Data.A->setX(MainIMU.getAccelX());
                Data.A->setY(MainIMU.getAccelY());
                Data.A->setZ(MainIMU.getAccelZ());
                gotAcc = true;
                break;

            case SENSOR_REPORTID_MAGNETIC_FIELD:
                Data.Magnet->setX(MainIMU.getMagX());
                Data.Magnet->setY(MainIMU.getMagY());
                Data.Magnet->setZ(MainIMU.getMagZ());
                gotMag = true;
                break;
        }

        if (gotQuat && gotGyro && gotAcc && gotMag) break;
    }

    unsigned long afterBNO = millis();

    auto isFiniteF = [&](float x) -> bool {
        return !(isnan(x) || isinf(x));
    };

    auto safeF = [&](float v) -> float {
        if (!isFiniteF(v)) return 0.0f;
        // optional clamp to reasonable range
        if (v > 1e6f) return 0.0f;
        if (v < -1e6f) return 0.0f;
        return v;
    };

    snprintf(buffer, sizeof(buffer),
        "||%lu|%.6f,%.6f,%.6f,%.6f|%.6f,%.6f,%.6f|"     // quat | mag
        "%.6f,%.6f,%.6f|%.6f,%.6f,%.6f|"               // pyyr | accel
        "%.6f,%.6f,%.6f|%lu||",                        // gyro | time
        beforeBNO,
        safeF(Data.quats->i), safeF(Data.quats->j), safeF(Data.quats->k), safeF(Data.quats->r),
        safeF(Data.Magnet->getX()), safeF(Data.Magnet->getY()), safeF(Data.Magnet->getZ()),
        safeF(Data.pyyr->getX()), safeF(Data.pyyr->getY()), safeF(Data.pyyr->getZ()),
        safeF(Data.A->getX()), safeF(Data.A->getY()), safeF(Data.A->getZ()),
        safeF(Data.omega->getX()), safeF(Data.omega->getY()), safeF(Data.omega->getZ()),
        afterBNO
    );

#ifdef MODE_TESTING
    Serial.println(buffer);
    Serial.println(strlen(buffer));
#endif

    return buffer; // stable static buffer
}

const char* IMUSensors::Update_Tared() {
    static char buffer[540];
    static Vector3D prevOmega(0,0,0);

    // clear flags at start of cycle
    bool gotQuat = false, gotGyro = false, gotAcc = false, gotMag = false;

    unsigned long startMs = millis();
    unsigned long beforeBNO = startMs;

    if (MainIMU.wasReset()) {
#ifdef MODE_TESTING
        Serial.println("sensor was reset");
#endif
        setReports();
        delay(200); // give it a moment
    }

    static unsigned long lastKeepAlive = 0;
    if (millis() - lastKeepAlive > 500) {
        MainIMU.enableReport(SENSOR_REPORTID_ROTATION_VECTOR, rotationVectorReportIntervalUs);
        lastKeepAlive = millis();
    }

    // Collect data set
    while (millis() - startMs < IMU_PACKET_TIMEOUT_MS) {
        if (!MainIMU.getSensorEvent()) { yield(); continue; }

        uint8_t id = MainIMU.getSensorEventID();

        switch (id) {
            case SENSOR_REPORTID_ROTATION_VECTOR:
                Data.quats->i = MainIMU.getQuatI() - Data.quats_init.i;
                Data.quats->j = MainIMU.getQuatJ() - Data.quats_init.j;
                Data.quats->k = MainIMU.getQuatK() - Data.quats_init.k;
                Data.quats->r = MainIMU.getQuatReal() - Data.quats_init.r;

                Data.pyyr->setX(MainIMU.getPitch() * 180.0f / PI - Data.pyyr_init.getX());
                Data.pyyr->setY(MainIMU.getYaw()   * 180.0f / PI - Data.pyyr_init.getY());
                Data.pyyr->setZ(MainIMU.getRoll()  * 180.0f / PI - Data.pyyr_init.getZ());
                gotQuat = true;
                break;

            case SENSOR_REPORTID_GYROSCOPE_CALIBRATED:
                Data.omega->setX(MainIMU.getGyroX() - Data.omega_init.getX());
                Data.omega->setY(MainIMU.getGyroY() - Data.omega_init.getY());
                Data.omega->setZ(MainIMU.getGyroZ() - Data.omega_init.getZ());
                gotGyro = true;
                break;

            case SENSOR_REPORTID_ACCELEROMETER:
                Data.A->setX(MainIMU.getAccelX() - Data.A_init.getX());
                Data.A->setY(MainIMU.getAccelY() - Data.A_init.getY());
                Data.A->setZ(MainIMU.getAccelZ() - Data.A_init.getZ());
                gotAcc = true;
                break;

            case SENSOR_REPORTID_MAGNETIC_FIELD:
                Data.Magnet->setX(MainIMU.getMagX() - Data.Magnet_init.getX());
                Data.Magnet->setY(MainIMU.getMagY() - Data.Magnet_init.getY());
                Data.Magnet->setZ(MainIMU.getMagZ() - Data.Magnet_init.getZ());
                gotMag = true;
                break;
        }

        if (gotQuat && gotGyro && gotAcc && gotMag) break;
    }

    unsigned long afterBNO = millis();

    auto isFiniteF = [&](float x) -> bool {
        return !(isnan(x) || isinf(x));
    };

    auto safeF = [&](float v) -> float {
        if (!isFiniteF(v)) return 0.0f;
        // optional clamp to reasonable range
        if (v > 1e6f) return 0.0f;
        if (v < -1e6f) return 0.0f;
        return v;
    };

    snprintf(buffer, sizeof(buffer),
        "||%lu|%.6f,%.6f,%.6f,%.6f|%.6f,%.6f,%.6f|"     // quat | mag
        "%.6f,%.6f,%.6f|%.6f,%.6f,%.6f|"               // pyyr | accel
        "%.6f,%.6f,%.6f|%lu||",                        // gyro | time
        beforeBNO,
        safeF(Data.quats->i), safeF(Data.quats->j), safeF(Data.quats->k), safeF(Data.quats->r),
        safeF(Data.Magnet->getX()), safeF(Data.Magnet->getY()), safeF(Data.Magnet->getZ()),
        safeF(Data.pyyr->getX()), safeF(Data.pyyr->getY()), safeF(Data.pyyr->getZ()),
        safeF(Data.A->getX()), safeF(Data.A->getY()), safeF(Data.A->getZ()),
        safeF(Data.omega->getX()), safeF(Data.omega->getY()), safeF(Data.omega->getZ()),
        afterBNO
    );

#ifdef MODE_TESTING
    Serial.println(buffer);
    Serial.println(strlen(buffer));
#endif

    return buffer; // stable static buffer
}