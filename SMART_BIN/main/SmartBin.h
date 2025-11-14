#ifndef SMART_BIN_H
#define SMART_BIN_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

extern LiquidCrystal_I2C lcd;

// GAS SENSOR
class GasSensor {
  public:
    GasSensor(int ledPin, int buzzerPin, int sensorPin);
    void setup();
    void update();
  private:
    int LED, Buzzer, Sensor;
    void printScreen(int value);
};

// IR COUNTER
class IRCounter {
  public:
    IRCounter(int irPin, int buzzerPin, int buttonPin);
    void setup();
    void update();
  private:
    int IRPin, buzzerPin, buttonPin;
    int count, oldValue;
    void Count();
    void Reset();
};

// PROXIMITY

class SmartBin {
  public:
    SmartBin(int gasLED, int gasBuzz, int gasAnalog,
             int irPin, int irBuzz, int irButton,
             int servoPin, int trigPin, int echoPin);
    void setup();
    void loop();

  private:
    GasSensor gasSensor;
    IRCounter irCounter;
    ProximityLid proxLid;
};

#endif


