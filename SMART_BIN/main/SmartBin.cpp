#include "SmartBin.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// GAS SENSOR
GasSensor::GasSensor(int ledPin, int sensorPin)
    : LED(ledPin), Sensor(sensorPin) {}

void GasSensor::setup() {
    pinMode(LED, OUTPUT);
}

void GasSensor::update() {
    value = analogRead(Sensor);
    Serial.print("Gas Reading: ");
    Serial.println(value);

    if (value > 3500) {
        digitalWrite(LED, HIGH);
        alert = true;
    } else {
        digitalWrite(LED, LOW);
        alert = false;
    }
}
// IR COUNTER
IRCounter::IRCounter(int irPin, int buttonPin)
    : IRPin(irPin), buttonPin(buttonPin) {
    oldValue = 1;
    lastBeepTime = 0;
}

void IRCounter::setup() {
    pinMode(IRPin, INPUT);
    pinMode(buttonPin, INPUT_PULLUP);
}

void IRCounter::update() {
    int irValue = digitalRead(IRPin);

    if (irValue == 0 && oldValue == 1) {
        oldValue = 0;
        Count();
        detectStart = millis();
    } 
    else if (irValue == 1 && oldValue == 0) {
        oldValue = 1;
        Serial.println("IR: No Object");
        detectStart = 0;
    }

    if (irValue == 0 && (millis() - detectStart >= 5000)) {
        binFull = true;
        Serial.println("BIN FULL DETECTED");
    }

    if (digitalRead(buttonPin) == LOW) {
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

// PROXIMITY 
ProximityLid::ProximityLid(int servoPin, int trigPin, int echoPin)
    : servoPin(servoPin), trigPin(trigPin), echoPin(echoPin) {}

void ProximityLid::setup() {
    myServo.attach(servoPin);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    myServo.write(0);
}

int ProximityLid::getDistance() {
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
            myServo.write(90);
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
}

void SmartBin::loop() {
    unsigned long currentMillis = millis();

    gasSensor.update();
    irCounter.update();
    proxLid.update(irCounter.binFull);

    gasSensorValue = gasSensor.value;
    distanceValue = proxLid.lastDistance;
    lidState = proxLid.getState();

    if (gasSensor.alert) {
        proxLid.myServo.write(90);
        lcd.setCursor(0, 0);
        lcd.print("GAS ALERT!");

        Serial.println("!!! GAS ALERT ACTIVE !!!");
        digitalWrite(buzzerPin, HIGH);
    }

    else if (irCounter.alert) {
        if (currentMillis - irCounter.lastBeepTime < 500) {
            digitalWrite(buzzerPin, HIGH);
        } else {
            digitalWrite(buzzerPin, LOW);
            irCounter.alert = false;
        }
    }

    else {
        digitalWrite(buzzerPin, LOW);
    }

    if (currentMode == AUTO && (currentMillis - lastLogTime >= 600000)) {  
        logs[logIndex].timestamp = currentMillis;
        logs[logIndex].gasValue = gasSensor.value;
        logs[logIndex].count = irCounter.count;
        logs[logIndex].distance = proxLid.lastDistance;
        logs[logIndex].binFull = irCounter.binFull;

        logIndex = (logIndex + 1) % 100;  
        lastLogTime = currentMillis;

        Serial.println("Log recorded.");
    }
}






