#include "Controls/Procedures.h"

void Procedures::MAINSETUP(bool quicksetup = false){
    // All pins default to an INPUT designation
    pinMode(31, OUTPUT); // PYRO
    digitalWrite(31, LOW);
#ifdef MODE_TESTING
    Serial.begin(115200);
    while (!Serial){;;}
#endif
    SPI.begin();
    if (!Wire.available()) Wire.begin();
    if (!Wire2.available()) Wire2.begin();

    DataCard->init();
    DataCard->SDWrite("SD Initialization.\n");

    if (!sensors->InitSensors()){ DataCard->SDWrite("One or multiple sensors failed!\n");
#ifdef MODE_TESTING
    Serial.println("One or multiple sensors failed!");
#endif
    }
#ifdef MODE_TESTING
    else Serial.println("All sensors initialized");
#endif
    Controller->Init(sensors);
    motors->init();

    const uint32_t init_millis = millis(); 
    int calibration_delay = 20000;
    if(quicksetup) calibration_delay = 100;
    while(millis() - init_millis < calibration_delay){ // Delay for calibration : 20s currently
        sensors->Update();
#ifdef MODE_TESTING
        Serial.println("Calibrating...");
#endif
    }

    // Initial positions. Tared positions 
    sensors->IMUs.Data.pyyr_init = sensors->IMUs.Data.pyyr_o;
    sensors->IMUs.Data.omega_init = sensors->IMUs.Data.omega_o;
    sensors->IMUs.Data.alpha_init = sensors->IMUs.Data.alpha_o;
    sensors->IMUs.Data.U_init = sensors->IMUs.Data.U_o;
    sensors->IMUs.Data.A_init = sensors->IMUs.Data.A_o;
    sensors->IMUs.Data.Magnet_init = sensors->IMUs.Data.Magnet_o;
    sensors->IMUs.Data.quats_init = sensors->IMUs.Data.quats_o;

    initial_height = sensors->MSSensor.Data.getMSData().getX();
    DataCard->SDWrite(" Initial Height: ");
    DataCard->SDWrite((String)initial_height);

    

    char buffer[540];
    snprintf(buffer, sizeof(buffer),
        "||%.3f,%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f||",
        sensors->IMUs.Data.quats_init.i,sensors->IMUs.Data.quats_init.j,sensors->IMUs.Data.quats_init.k,sensors->IMUs.Data.quats_init.r,
        sensors->IMUs.Data.Magnet_init.getX(), sensors->IMUs.Data.Magnet_init.getY(), sensors->IMUs.Data.Magnet_init.getZ(),
        sensors->IMUs.Data.pyyr_init.getX(), sensors->IMUs.Data.pyyr_init.getY(), sensors->IMUs.Data.pyyr_init.getZ(),
        sensors->IMUs.Data.A_init.getX(), sensors->IMUs.Data.A_init.getY(), sensors->IMUs.Data.A_init.getZ(),
        sensors->IMUs.Data.omega_init.getX(), sensors->IMUs.Data.omega_init.getY(), sensors->IMUs.Data.omega_init.getZ(),
        sensors->IMUs.Data.alpha_init.getX(), sensors->IMUs.Data.alpha_init.getY(), sensors->IMUs.Data.alpha_init.getZ()
    );

    DataCard->SDWrite(" Initial IMU data: ");
    DataCard->SDWrite(String(buffer));

#ifdef MODE_TESTING
    Serial.print("\nInitial IMU data: ");
    Serial.println((String)buffer);
    Serial.print("Initial Height: ");
    Serial.println(initial_height);
#endif
}

void Procedures::DRIVE_CONTROLS(bool LAUNCHED){
    DataCard->SDWrite(String(sensors->Update_Tared(initial_height)));
    //DataCard->SDWrite(String(Controller->Update(motors)));
    if (LAUNCHED){
        if (updateMotorSequence()) DataCard->SDWrite("Motor Movement");
    }
}

bool Procedures::updateMotorSequence() {
    static uint32_t stepStartTime = millis();
    uint32_t now = millis();

    if (seqIndex < 13) {
        if (now - stepStartTime >= sequence[seqIndex].duration) {
            motors->setStateTo(sequence[seqIndex].values);
            stepStartTime = now;
            seqIndex++;
            return true;
        }
        return false;
    }
    return false;
}

// Cross examined 11/08/2025. No bugs found
bool Procedures::EJECTION() {
    // --- configurable thresholds/timings (tune as needed) ---
    const double START_HEIGHT_THRESHOLD = 10.0;    // start arming (units: meters)
    const double START_VEL_THRESHOLD    = 20.0;    // start arming (units: m/s)
    const double START_ACC_THRESHOLD    = 50.0;    // start arming (units: m/s^2)

    const int TRUE_CT_REQUIRED = 11;               // number of consecutive samples required to arm
    const double EJECT_HEIGHT_THRESHOLD = 950.0;   // ejection altitude (meters)
    const double EJECT_VEL_THRESHOLD    = 6.0;    // ejection velocity threshold (m/s)
    const double EJECT_ACC_THRESHOLD    = 30.0;    // acceleration threshold for ejection (m/s^2)

    const uint32_t VELOCITY_SAMPLE_MS   = 50;      // compute velocity every ~50 ms
    const uint32_t GLOBAL_FAILSAFE_MS   = 14000;   // if nothing fired after this since arm -> force eject
    const uint32_t EJECT_PULSE_MS       = 5000;    // keep actuator pin HIGH this long

    // --- persistent state (static so preserved across calls from loop) ---
    static uint32_t prevtime = millis();
    static double prevHeight = 0.0;
    static float velocity = 0.0f;

    static int TRUE_CT = 0;
    static bool launched_local = false; // internal local mirror/flag

    // explicit state machine for ejection
    enum EjectState { IDLE = 0, ARMED = 1, EJECT_PIN_ON = 2, EJECT_DONE = 3 };
    static EjectState state = IDLE;

    // timing for state transitions
    static uint32_t armStartTime = 0;       // set when entering ARMED
    static uint32_t ejectPulseStart = 0;    // set when actuator pin set HIGH

    // --- read sensors & compute height/velocity safely ---
    double height = sensors->MSSensor.Data.getMSData().getX() - initial_height; // meters
    uint32_t now = millis();

    // Update velocity at controlled interval using floating point seconds (prevent div-by-zero)
    if ((uint32_t)(now - prevtime) >= VELOCITY_SAMPLE_MS) {
        double dt_ms = (double)(now - prevtime);
        double dt_s = dt_ms / 1000.0; // seconds
        if (dt_s > 0.0) {
            velocity = (float)((height - prevHeight) / dt_s);
        } else {
            velocity = 0.0f;
        }

        // Log velocity to SD
        char buf[64];
        int n = snprintf(buf, sizeof(buf), "Velocity: %.2f m/s\n", velocity);
        if (n > 0) DataCard->SDWrite(String(buf));

        prevHeight = height;
        prevtime = now;
    }

        // --- ARM logic (timing-based noise rejection) ---
    static uint32_t armConditionStart = 0;
    const uint32_t ARM_HOLD_MS = 150;   // require conditions to hold for 150ms

    bool armConditions =
        (height > START_HEIGHT_THRESHOLD) &&
        (velocity > START_VEL_THRESHOLD) &&
        (sensors->IMUs.Data.A->magnitude() > START_ACC_THRESHOLD);  // <-- fixed A access

    if (state == IDLE) {
        if (armConditions) {
            if (armConditionStart == 0) armConditionStart = now; // begin hold timer
            if ((now - armConditionStart) > ARM_HOLD_MS) {
                state = ARMED;
                armStartTime = now;   // timestamp for failsafe timer
                launched = true;      // external flight flag

                DataCard->SDWrite("\nEJECTION: ARMED\n");
    #ifdef MODE_TESTING
                Serial.println("EJECTION: ARMED");
    #endif
            }
        } else {
            armConditionStart = 0; // reset timer if conditions break
        }
    }


    // when True count threshold crossed, move to ARMED (only once)
    if ((TRUE_CT >= TRUE_CT_REQUIRED) && (state == IDLE)) {
        state = ARMED;
        armStartTime = now; // set *once*
        launched = true;     // external flag you already use
        // telemetry
        DataCard->SDWrite("\nEJECTION: ARMED\n");
#ifdef MODE_TESTING
        Serial.println("EJECTION: ARMED");
#endif
    }

    // --- state machine behavior ---
    switch (state) {
        case IDLE:
            // nothing else to do
            break;

        case ARMED:
            // (1) Check the nominal ejection condition
            if ((velocity < EJECT_VEL_THRESHOLD)
                && (height >= EJECT_HEIGHT_THRESHOLD)
                && (sensors->IMUs.Data.A->magnitude() < EJECT_ACC_THRESHOLD)) {

                // fire actuator
                digitalWrite(31, HIGH);
                ejectPulseStart = now;
                state = EJECT_PIN_ON;

                DataCard->SDWrite("\n---------- EJECTION!!!! EXPECTED ONE -----------\n");
#ifdef MODE_TESTING
                Serial.println("EJECT (expected)");
#endif
                break;
            }

            // (2) Global failsafe: if too much time passes while armed, fire anyway
            if ((uint32_t)(now - armStartTime) > GLOBAL_FAILSAFE_MS) {
                digitalWrite(31, HIGH);
                ejectPulseStart = now;
                state = EJECT_PIN_ON;

                DataCard->SDWrite("\nEJECT FAILSAFE TRIGGERED!!!\n");
#ifdef MODE_TESTING
                Serial.println("EJECT FAILSAFE");
#endif
            }
            break;

        case EJECT_PIN_ON:
            // keep pin HIGH for the pulse period, then release and finalize
            if ((uint32_t)(now - ejectPulseStart) >= EJECT_PULSE_MS) {
                digitalWrite(31, LOW);
                DataCard->SDWrite("EJECTION: PIN RELEASED\n");
#ifdef MODE_TESTING
                Serial.println("EJECTION: PIN RELEASED");
#endif
                DataCard->close(); // if you want to close the SD after ejection
                state = EJECT_DONE;
                launched_local = true; // local flag we can use if needed
            }
            break;

        case EJECT_DONE:
            // ejection already complete; remain here. Return true once to caller.
            break;
    }

    // Return true if ejection completed (pin went HIGH and pulse ended)
    return (state == EJECT_DONE);
}

Servos* Procedures::getServoAddr(){
    return this->motors;
}

void Procedures::stopData(){
    DataCard->close();
}