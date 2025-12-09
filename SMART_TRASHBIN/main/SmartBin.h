#ifndef SMART_BIN_H
#define SMART_BIN_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

#include "Sensors.h"
#include "Globals.h"

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
