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
    IRCounter(int irPin, int buttonPin);

    void setup();
    void update();

    bool alert = false;
    bool binFull = false;

    unsigned long lastBeepTime;
    unsigned long detectStart = 0;

    int count = 0;

    void resetAll();

    int IRPin, buttonPin;

private:
    int oldValue;
    void Count();
};

// PROXIMITY
class ProximityLid {
  public:
    ProximityLid(int servoPin, int trigPin, int echoPin);
    void setup();
    void update();
  private:
    Servo myServo;
    int servoPin, trigPin, echoPin;
    int distance;
    int getDistance();
};

class SmartBin {
public:
    enum Mode { AUTO, MAINTENANCE, SLEEP };  

    SmartBin(
        int gasLED, int gasAnalog,
        int irPin, int irButton,
        int buzzerPin,
        int servoPin, int trigPin, int echoPin
    );

    void setup();
    void loop();

    void setMode(Mode m) { currentMode = m; }  
    Mode getMode() { return currentMode; }     

    int gasSensorValue = 0;
    int distanceValue = 0;
    String lidState = "CLOSED";

    GasSensor gasSensor;
    IRCounter irCounter;
    ProximityLid proxLid;

    int buzzerPin;

    LogEntry logs[100];  
    int logIndex = 0;    
    unsigned long lastLogTime = 0;  

private:
    Mode currentMode = AUTO;  
};

#endif




