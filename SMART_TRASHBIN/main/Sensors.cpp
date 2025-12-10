#include "Sensors.h"
#include <EEPROM.h>  

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define EEPROM_MAGIC 0xABCD1234

GasSensor::GasSensor(int ledPin, int sensorPin)
    : LED(ledPin), Sensor(sensorPin) {}

void GasSensor::setup() {
    if (LED >= 0) pinMode(LED, OUTPUT);
}

void GasSensor::update() {
    if (Sensor < 0) return;
    value = analogRead(Sensor);
    Serial.print("Gas Reading: ");
    Serial.println(value);

    if (value > 3000) {
        if (LED >= 0) digitalWrite(LED, HIGH);
        alert = true;
    } else {
        if (LED >= 0) digitalWrite(LED, LOW);
        alert = false;
    }
}

IRCounter::IRCounter(int irPin, int buttonPin)
    : IRPin(irPin), buttonPin(buttonPin) {
    oldValue = 1;
    lastBeepTime = 0;
}

void IRCounter::setup() {
    if (IRPin >= 0) pinMode(IRPin, INPUT);
    if (buttonPin >= 0) pinMode(buttonPin, INPUT_PULLUP);
}

void IRCounter::update() {
    if (IRPin < 0) return;
    int irValue = digitalRead(IRPin);

    if (irValue == 0 && oldValue == 1) {
        oldValue = 0;
        Count();
        detectStart = millis();
    } else if (irValue == 1 && oldValue == 0) {
        oldValue = 1;
        Serial.println("IR: No Object");
        detectStart = 0;
    }

    if (irValue == 0 && (millis() - detectStart >= 5000)) {
        binFull = true;
        Serial.println("BIN FULL DETECTED");
    }

    if (buttonPin >= 0 && digitalRead(buttonPin) == LOW) {
        resetAll();
    }

    lcd.setCursor(0, 1);
    if (binFull) {
        lcd.print("FULL ");
    } else {
        lcd.print("Trash Count: ");
        lcd.print(count);
        lcd.print(" ");
    }
}

void IRCounter::Count() {
    count++;
    alert = true;
    lastBeepTime = millis();

    Serial.println("IR: Object Detected");
    Serial.print("Trash Count: ");
    Serial.println(count);
}

void IRCounter::resetAll() {
    count = 0;
    binFull = false;
    alert = false;
    detectStart = 0;

    Serial.println("RESET -> Normal Mode");
}

ProximityLid::ProximityLid(int servoPin, int trigPin, int echoPin)
    : servoPin(servoPin), trigPin(trigPin), echoPin(echoPin) {}

void ProximityLid::setup() {
    if (servoPin >= 0) myServo.attach(servoPin);

    if (trigPin >= 0) pinMode(trigPin, OUTPUT);
    if (echoPin >= 0) pinMode(echoPin, INPUT);

    myServo.write(0);
}

int ProximityLid::getDistance() {
    if (trigPin < 0 || echoPin < 0) return 0;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 25000);
    return duration * 0.0343 / 2;
}

void ProximityLid::update(bool binFull) {
    lastDistance = getDistance();

    Serial.print("Distance: ");
    Serial.print(lastDistance);
    Serial.println(" cm");

    String state = (!binFull && lastDistance <= 25) ? "OPEN" : "CLOSED";

    if (state != lastState) {
        if (state == "OPEN")
            myServo.write(180);
        else
            myServo.write(0);

        lcd.setCursor(0, 0);
        lcd.print(state + " ");

        lastState = state;
    }
}

SmartBin::SmartBin(
    int gasLED, int gasAnalog,
    int irPin, int irButton,
    int buzzerPin,
    int servoPin, int trigPin, int echoPin
)
    : gasSensor(gasLED, gasAnalog),
      irCounter(irPin, irButton),
      proxLid(servoPin, trigPin, echoPin),
      buzzerPin(buzzerPin) {}

void SmartBin::setup() {
    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(" Smart Bin Ready ");
    delay(1500);
    lcd.clear();

    pinMode(buzzerPin, OUTPUT);

    gasSensor.setup();
    irCounter.setup();
    proxLid.setup();

    EEPROM.begin(1704);

    loadLogsFromEEPROM();
}

void SmartBin::loop() {
    unsigned long now = millis();

    switch (currentMode) {

        case AUTO: {
            static bool wasMaintenance = false;
            if (wasMaintenance) {
                lcd.clear();
                wasMaintenance = false;
            }

            gasSensor.update();
            irCounter.update();
            proxLid.update(irCounter.binFull);

            gasSensorValue = gasSensor.value;
            distanceValue = proxLid.lastDistance;
            lidState = proxLid.getState();

            if (gasSensor.alert) {
                proxLid.myServo.write(180);
                lcd.setCursor(0, 0);
                lcd.print("GAS ALERT!     ");

                digitalWrite(buzzerPin, HIGH);
            }

            else if (irCounter.alert) {
                if (now - irCounter.lastBeepTime < 500) {
                    digitalWrite(buzzerPin, HIGH);
                } else {
                    digitalWrite(buzzerPin, LOW);
                    irCounter.alert = false;
                }
            }

            else {
                digitalWrite(buzzerPin, LOW);
            }

            if (now - lastLogTime >= 1200000) {
                logs[logIndex].timestamp = now;
                logs[logIndex].gasValue = gasSensor.value;
                logs[logIndex].count = irCounter.count;
                logs[logIndex].distance = proxLid.lastDistance;
                logs[logIndex].binFull = irCounter.binFull;

                logIndex = (logIndex + 1) % 100;
                lastLogTime = now;

                saveLogsToEEPROM();

                Serial.println("Log recorded and saved to EEPROM.");
            }

            break;
        }

        case MAINTENANCE: {
            digitalWrite(buzzerPin, LOW);
            if (gasSensor.LED >= 0) digitalWrite(gasSensor.LED, LOW);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("MAINTENANCE");
            break;
        }

        case SLEEP: {
            gasSensor.update();     
            irCounter.binFull = false;
            irCounter.alert = false;

            proxLid.myServo.write(0);

            lcd.clear();
            lcd.noBacklight();

            digitalWrite(buzzerPin, LOW);

            break;
        }
    }
}

void SmartBin::setMode(Mode m) {
    currentMode = m;
    if (m == MAINTENANCE) {
        proxLid.myServo.write(0);
    }
}

void SmartBin::saveLogsToEEPROM() {
    int addr = 0;
    EEPROM.put(addr, EEPROM_MAGIC);
    addr += sizeof(unsigned long);  

    EEPROM.put(addr, logIndex);
    addr += sizeof(int);  

    for (int i = 0; i < 100; i++) {
        EEPROM.put(addr, logs[i]);
        addr += sizeof(LogEntry);
    }

    EEPROM.commit();
    Serial.println("Logs saved to EEPROM with validation.");
}

void SmartBin::loadLogsFromEEPROM() {
    int addr = 0;
    unsigned long magic;
    EEPROM.get(addr, magic);
    addr += sizeof(unsigned long);

    if (magic == EEPROM_MAGIC) {
        EEPROM.get(addr, logIndex);
        addr += sizeof(int);

        for (int i = 0; i < 100; i++) {
            EEPROM.get(addr, logs[i]);
            addr += sizeof(LogEntry);
        }
        Serial.println("Valid logs loaded from EEPROM.");
    } else {
        logIndex = 0;
        memset(logs, 0, sizeof(logs));  
        Serial.println("EEPROM uninitialized or corrupted. Logs reset to empty.");
        saveLogsToEEPROM();
    }
}

void SmartBin::clearEEPROM() {
    for (int i = 0; i < 1704; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    Serial.println("EEPROM cleared.");
}
