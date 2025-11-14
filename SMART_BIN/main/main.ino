#include "SmartBin.h"

// GAS SENSOR
#define GAS_LED_PIN      23
#define GAS_BUZZER_PIN   14
#define GAS_ANALOG_PIN   34

// IR COUNTER
#define IR_PIN           33
#define IR_BUZZER_PIN    27
#define IR_BUTTON_PIN    26

// PROXIMITY 
#define SERVO_PIN        5
#define TRIG_PIN         18
#define ECHO_PIN         19

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

