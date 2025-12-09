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

#endif
