#include "SmartBin.h"

// GAS SENSOR
const int GAS_LED_PIN = 23;
const int GAS_BUZZER_PIN = 14;
const int GAS_ANALOG_PIN = 34;

// IR COUNTER
const int IR_PIN = 33
const int IR_BUZZER_PIN = 27
const int IR_BUTTON_PIN = 26

// PROXIMITY 
const int SERVO_PIN = 5
const int TRIG_PIN = 18
const int ECHO_PIN = 19

SmartBin smartBin(
  GAS_LED_PIN, GAS_BUZZER_PIN, GAS_ANALOG_PIN,
  IR_PIN, IR_BUZZER_PIN, IR_BUTTON_PIN,
  SERVO_PIN, TRIG_PIN, ECHO_PIN
);

void setup() {
  Serial.begin(115200);
  smartBin.setup();
}

void loop() {
  smartBin.loop();
}

