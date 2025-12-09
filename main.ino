#include "SmartBin.h"
#include "CommProtocol.h"

const char* ssid = "danieles";
const char* password = "daniel290312";

const int GAS_LED_PIN = 14;
const int GAS_ANALOG_PIN = 34;

const int IR_PIN = 4;
const int IR_BUTTON_PIN = 15;

const int SERVO_PIN = 13;
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;

const int SHARED_BUZZER_PIN = 27;

SmartBin smartBin(
    GAS_LED_PIN,
    GAS_ANALOG_PIN,
    IR_PIN,
    IR_BUTTON_PIN,
    SHARED_BUZZER_PIN,
    SERVO_PIN,
    TRIG_PIN,
    ECHO_PIN
);

CommProtocol comm(ssid, password, 80);

void setup() {
    Serial.begin(115200);

    int unusedPins[] = {16, 17, 19, 23, 25, 26, 32, 33};
    for (int i = 0; i < 8; i++) {
        pinMode(unusedPins[i], INPUT_PULLDOWN);  
    }

    pinMode(35, INPUT_PULLDOWN);
    pinMode(36, INPUT_PULLDOWN);
    pinMode(39, INPUT_PULLDOWN);

    smartBin.setup();

    comm.begin();
}

void loop() {
    smartBin.loop();
    comm.handleClient(smartBin);
}
