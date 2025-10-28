#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SensorControl.h"  

const int servoPin = 5;
const int trigPin = 18;
const int echoPin = 19;

SensorControl sensorControl(servoPin, trigPin, echoPin);  

void setup() {
  Serial.begin(115200);
  sensorControl.setup();
}

void loop() {
  sensorControl.loop();
}
