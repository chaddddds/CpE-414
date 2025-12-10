#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "Globals.h"

class GasSensor {
public:
    GasSensor(int ledPin = -1, int sensorPin = -1);

    void setup();
    void update();

    bool alert = false;
    int LED, Sensor;
    int value = 0;
};

class IRCounter {
public:
    IRCounter(int irPin = -1, int buttonPin = -1);

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

class ProximityLid {
public:
    ProximityLid(int servoPin = -1, int trigPin = -1, int echoPin = -1);

    void setup();
    void update(bool binFull);
    int getDistance();

    Servo myServo;
    int lastDistance = 0;

    int servoPin, trigPin, echoPin;

    String getState() { return lastState; }

private:
    int distance;
    String lastState = "CLOSED";
};

struct LogEntry {
    unsigned long timestamp;
    int gasValue;
    int count;
    int distance;
    bool binFull;
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

    void setMode(Mode m);
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

    void saveLogsToEEPROM();
    void loadLogsFromEEPROM();
    void clearEEPROM();  

private:
    Mode currentMode = AUTO;
};

#endif
