#include "Sensors.h"

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
